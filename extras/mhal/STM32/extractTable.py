import pdfplumber
import pandas as pd
import re
import csv

# PDF-Datei öffnen
def extractAlternateFunction(filename, startPage):
	with pdfplumber.open(filename) as pdf:
		data = []
		for page_num in range(startPage, min(len(pdf.pages),startPage+10)):
			try:
				page = pdf.pages[page_num]
				print(page.extract_text()[:500])
				print(f"Verarbeite Seite {page_num}...")		

				tables = page.extract_tables()
				if tables:
					for table in tables:
						for row in table:
							if not row[1]:
								continue
							GPIO = row[1]
							if GPIO:
								if re.match(r'^P[ABCDEFG]\d+$', GPIO):
									defs = row[2::]
									newLine = []
									for d in defs:
										d = re.sub(r'\s+', '', d)
										newLine.append(d)
									data.append([GPIO] + newLine)
							
					print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
					print(data)
					print("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
			except:
				pass
		return data

if 1==2:
	data = extractAlternateFunction("stm32g474re.pdf",72)
	with open('alternateFuncs.csv', 'w', newline='', encoding='utf-8') as f:
		writer = csv.writer(f)
		writer.writerows(data)
else:
	with open('alternateFuncs.csv', newline='', encoding='utf-8') as f:
		reader = csv.reader(f)
		data = list(reader)

PORT_REF = {
	'PA' : 'GPIOA_BASE',
	'PB' : 'GPIOB_BASE',
	'PC' : 'GPIOC_BASE',
	'PD' : 'GPIOD_BASE',
	'PE' : 'GPIOE_BASE',
	'PF' : 'GPIOF_BASE',
	'PG' : 'GPIOG_BASE'
}

class Tfunc():
	def __init__(self, _name, _id):
		self.name = _name
		self.id = _id
	
	def __repr__(self):
		return f"Tfunc(name='{self.name}', id='{self.id}')"
	

class Tref():
	REF_CONST = 10			#0-10 for Timer Channels
	
	def __init__(self):
		self.Fresponsible = False
		self.name = ""
		self.base = ""
		self.FuncList = []
		
	def parse(self, _per, _func):
		self.Fresponsible = type(self).RE_PER.match(_per) and self.parseFunc(_func)
		if self.Fresponsible:
			self.name = _per
			self.base = _per + '_BASE'
			return True
	
	def isResponsible(self):
		return self.Fresponsible

	def getCondition(self):
		return f" && ({SYNTAX_FA_AF}=={self.ScopedName})"

	def addFunc(self,_func):
		scopedName = f"{self.FAM_NAME}::{_func}"
		self.ScopedName = scopedName
		for f in self.FuncList:
			if f.name == _func:
				return
		print('addFunc: ' + _func)
		f = Tfunc(_func,__class__.REF_CONST)
		self.FuncList.append(f)		
		__class__.REF_CONST+=1
		
	def writeSpecialConsts(self, file):
		pass

	def writeConsts(self, file):
		file.nextIndent(f'namespace {self.FAM_NAME}' + '{')
		self.writeSpecialConsts(file)
		for func in self.FuncList:
			print(f'{func.name} = {func.id}')
			f.write(f'constexpr static auto {func.name} = {func.id};\n')
		file.prevIndent('}')
	
	
class TimerRef(Tref):
	RE_PER = re.compile(r'^TIM\d+$')

	def __init__(self, *args, **kwargs):	
		super().__init__(*args, **kwargs)
		self.HIGHEST_CHANNEL = -1
		self.FAM_NAME = 'TIM'
		self.RE_FUNC = re.compile(r'^CH([123456])$')
		self.RE_FUNC = re.compile(
			r'^CH(?P<chclassic>[0-6])$'          # CH0 bis CH6 → klassisch
			r'|^(?P<chext>CH\d+n?)$'
			r'|^(?P<bkin>BKIN\d*)$'
		)		
		
	#CH1
	def parseFunc(self, _str):
		match = self.RE_FUNC.match(_str)
		if match:
			if match.group("chclassic"):
				self.channel = int(match.group("chclassic"))
				self.ScopedName = f'{self.FAM_NAME}::CH{self.channel}'
				if self.channel > self.HIGHEST_CHANNEL:
					self.HIGHEST_CHANNEL = self.channel
			else:
				self.ScopedName = func
				self.addFunc(func)
			return True

	def writeSpecialConsts(self, file):
		for i in range(1,self.HIGHEST_CHANNEL+1):
			f.write(f'constexpr static auto CH{i} = {i};\n')
		
	def __repr__(self):
		return f"TimerRef(name='{self.name}', base='{self.base}')"


class TusartRef(Tref):
	FuncList = []
	RE_PER = re.compile(r'^USART\d+$')
	FAM_NAME = 'USART'

	def parseFunc(self,_str):
		if _str == "RX" or  _str == "TX":
			self.addFunc(_str)
			return True
		else:
			return False
		
	def __repr__(self):
		return f"TusartRef(name='{self.name}', base='{self.base}')"


class Ti2c(Tref):
	FuncList = []
	RE_PER = re.compile(r'^I2C\d+$')
	FAM_NAME = 'I2C'

	def parseFunc(self,_str):
		if _str == "SDA" or  _str == "SCL":
			self.addFunc(_str)
			return True
		else:
			return False
		
	def __repr__(self):
		return f"TusartRef(name='{self.name}', base='{self.base}')"


RE_PERIPHS = [TimerRef(),TusartRef(),Ti2c()]
def getPeripheral(_per, _func):
	for p in RE_PERIPHS:
		if p.parse(_per, _func):
			return p

		
	
################################################
#configure syntax:
################################################
''' 
if (GPIO_BASE_ADDR == GPIOA_BASE){
	if (_GPIO_PIN == 0){
		if (_p == TIM2_BASE && (_s==1)) return LL_GPIO_AF_1;
	}
'''
#arguments to getAlternateFunction(_port,_pin,_per,_func)
SYNTAX_FA_PORT = '_port'
SYNTAX_FA_PIN = '_pin'
SYNTAX_FA_PERIPHERAL = "_per"
SYNTAX_FA_AF = "_af"

CODE_AF_PREFIX = "LL_GPIO_AF_"
SYNTAX_INCLUDE_GUARD = "MHAL_AF_STM32_H_"

################################################

lastPort = ""
lines = []
baseAddrMatchBlock = []
gpioPinMatchBlock = []
func_collection = []
for d in data:
	pin = d[0]								#PA0
	port = pin[:2] if len(d) >= 2 else ""	#PA
	pin = pin[2:]							#0..15
	if not pin.isdigit():
		continue
	if not port in PORT_REF:
		continue
	port = PORT_REF[port]					#PA->GPIOA_BASE
	AFs = d[1:]
	
	if port != lastPort:
		lastPort = port
		if len(baseAddrMatchBlock)>1:
			lines = lines + baseAddrMatchBlock + ["}"]
		
		#if (GPIO_BASE_ADDR == GPIOA_BASE){
		line = f"if ({SYNTAX_FA_PORT} == {port})" + "{"
		baseAddrMatchBlock = [line]
	
	for index, AF in enumerate(AFs):
		#try:
		lst = AF.split('/')					#TIM2_CH1/TIM2_ETR -> [TIM2_CH1,TIM2_ETR]
		for AF in lst:
			if AF == "-":
				continue
			if '_' in AF:
				per, func = AF.split('_', 1)	#per=TIM1  func=CH1
			else:	
				#here we can only end up with AF=EVENTOUT???
				per, func = AF, ""
				continue
			pObj = getPeripheral(per,func)
			if not pObj:
				#print(f'exclude {per}_{func}')
				continue
				
			#if (_p == TIM2_BASE && (_s==CH1)) return LL_GPIO_AF_1;
			gpioPinMatchBlock.append(f"\t\tif ({SYNTAX_FA_PERIPHERAL} == {pObj.base}{pObj.getCondition()}) return {CODE_AF_PREFIX}{index};")
			
		#except Exception as e:
		#	print(e)
		#	raise

	#if (_GPIO_PIN == 12){
	# ...gpioPinMatchBlock
	#}
	if len(gpioPinMatchBlock)>0:
		baseAddrMatchBlock += [f"\tif ({SYNTAX_FA_PIN} == {pin})" + "{"]
		baseAddrMatchBlock += gpioPinMatchBlock
		baseAddrMatchBlock += ['\t}']
		gpioPinMatchBlock = []

if len(baseAddrMatchBlock)>1:
	lines = lines + baseAddrMatchBlock + ["}"]
	
class TcodeWriter:
	def __init__(self,_filename):
		self.indent = 0
		self.file = open(_filename, "w", encoding="utf-8")
	
	def write(self,_str):
		s = ''
		for i in range(self.indent):
			s += '\t'
		self.file.write(s + _str)
	
	def writeln(self,_str):
		self.write(_str + '\n')
		
	def nextIndent(self,_str):
		self.writeln(_str)
		self.indent += 1
		
	def prevIndent(self,_str):
		self.indent -= 1
		self.writeln(_str)
		
f = TcodeWriter('d:/dropbox/projects/MMS/firmware/system/sdds/src/mhal/STM32/uAF.h')
f.writeln(f"#ifndef {SYNTAX_INCLUDE_GUARD}")
f.writeln(f"#define {SYNTAX_INCLUDE_GUARD}")
f.writeln("")

f.nextIndent('namespace mhal{')
f.nextIndent('namespace AF{')
for p in RE_PERIPHS:
	print("-> PERIPHS " + p.FAM_NAME)
	pInst = p.writeConsts(f)
	print("<- PERIPHS")
	
f.writeln('')

f.nextIndent(f'constexpr static int32_t getAlternateFunction(uint32_t {SYNTAX_FA_PORT}, uint32_t {SYNTAX_FA_PIN}, uintptr_t {SYNTAX_FA_PERIPHERAL}, uint32_t {SYNTAX_FA_AF})' + '{')
for l in lines:
	f.write(l + "\n")
f.prevIndent('}')	#f.nextIndent(f'constexpr static int32_t getAlternateFunction(uintptr_t {SYNTAX_FA_PERIPHERAL}, uint32_t {SYNTAX_FA_AF})' + '{')

f.prevIndent('}')	#f.nextIndent('namespace mhal{')
f.prevIndent('}')	#f.nextIndent('namespace AF{')

f.writeln("")
f.writeln(f"#endif //{SYNTAX_INCLUDE_GUARD}")

