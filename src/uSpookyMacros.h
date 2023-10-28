#ifndef USPOOKYMACROS_h
#define USPOOKYMACROS_h

/************************************************************************************
uSpookyMacros.h

Auto generated file by C++ Spooky Macro Generator
	Author: Mark Lamneck
	Creation Date: 20.10.23
	macros with preceding __ are for internal use

Macros to be used:
	I. SP_COUNT_VARARGS(...):
		macro to count the number of arguments given to a variadic macro (up to 254)
		SP_COUNT_VARARGS(A,B,C) -> 3
		SP_COUNT_VARARGS(5,4,3,2,1) -> 5

	II. SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(x,...):
		call macro x for all params in ... (for up to 254 arguments) like:
		for param in ... do x(param) where param is a single value
		example: given a macro: #define to_comma_sep_list(_a) _a,
			SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(to_comma_sep_list, A, B, C) -> A,B,C,
************************************************************************************/

#define __SP_GET_ARG_256( \
	_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,_17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32, \
	_33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48,_49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,_64, \
	_65,_66,_67,_68,_69,_70,_71,_72,_73,_74,_75,_76,_77,_78,_79,_80,_81,_82,_83,_84,_85,_86,_87,_88,_89,_90,_91,_92,_93,_94,_95,_96, \
	_97,_98,_99,_100,_101,_102,_103,_104,_105,_106,_107,_108,_109,_110,_111,_112,_113,_114,_115,_116,_117,_118,_119,_120,_121,_122,_123,_124,_125,_126,_127,_128, \
	_129,_130,_131,_132,_133,_134,_135,_136,_137,_138,_139,_140,_141,_142,_143,_144,_145,_146,_147,_148,_149,_150,_151,_152,_153,_154,_155,_156,_157,_158,_159,_160, \
	_161,_162,_163,_164,_165,_166,_167,_168,_169,_170,_171,_172,_173,_174,_175,_176,_177,_178,_179,_180,_181,_182,_183,_184,_185,_186,_187,_188,_189,_190,_191,_192, \
	_193,_194,_195,_196,_197,_198,_199,_200,_201,_202,_203,_204,_205,_206,_207,_208,_209,_210,_211,_212,_213,_214,_215,_216,_217,_218,_219,_220,_221,_222,_223,_224, \
	_225,_226,_227,_228,_229,_230,_231,_232,_233,_234,_235,_236,_237,_238,_239,_240,_241,_242,_243,_244,_245,_246,_247,_248,_249,_250,_251,_252,_253,_254,_255,N, ...) N

#define SP_COUNT_VARARGS(...) __SP_GET_ARG_256("ignored", ##__VA_ARGS__ \
	,254,253,252,251,250,249,248,247,246,245,244,243,242,241,240,239,238,237,236,235,234,233,232,231,230,229,228,227,226,225 \
	,224,223,222,221,220,219,218,217,216,215,214,213,212,211,210,209,208,207,206,205,204,203,202,201,200,199,198,197,196,195,194,193 \
	,192,191,190,189,188,187,186,185,184,183,182,181,180,179,178,177,176,175,174,173,172,171,170,169,168,167,166,165,164,163,162,161 \
	,160,159,158,157,156,155,154,153,152,151,150,149,148,147,146,145,144,143,142,141,140,139,138,137,136,135,134,133,132,131,130,129 \
	,128,127,126,125,124,123,122,121,120,119,118,117,116,115,114,113,112,111,110,109,108,107,106,105,104,103,102,101,100,99,98,97 \
	,96,95,94,93,92,91,90,89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65 \
	,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33 \
	,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1 \
	,0)

#define SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(x, ...) __SP_GET_ARG_256("ignored", ##__VA_ARGS__ \
	,__rmc_254,__rmc_253,__rmc_252,__rmc_251,__rmc_250,__rmc_249,__rmc_248,__rmc_247,__rmc_246,__rmc_245,__rmc_244,__rmc_243,__rmc_242,__rmc_241 \
	,__rmc_240,__rmc_239,__rmc_238,__rmc_237,__rmc_236,__rmc_235,__rmc_234,__rmc_233,__rmc_232,__rmc_231,__rmc_230,__rmc_229,__rmc_228,__rmc_227,__rmc_226,__rmc_225 \
	,__rmc_224,__rmc_223,__rmc_222,__rmc_221,__rmc_220,__rmc_219,__rmc_218,__rmc_217,__rmc_216,__rmc_215,__rmc_214,__rmc_213,__rmc_212,__rmc_211,__rmc_210,__rmc_209 \
	,__rmc_208,__rmc_207,__rmc_206,__rmc_205,__rmc_204,__rmc_203,__rmc_202,__rmc_201,__rmc_200,__rmc_199,__rmc_198,__rmc_197,__rmc_196,__rmc_195,__rmc_194,__rmc_193 \
	,__rmc_192,__rmc_191,__rmc_190,__rmc_189,__rmc_188,__rmc_187,__rmc_186,__rmc_185,__rmc_184,__rmc_183,__rmc_182,__rmc_181,__rmc_180,__rmc_179,__rmc_178,__rmc_177 \
	,__rmc_176,__rmc_175,__rmc_174,__rmc_173,__rmc_172,__rmc_171,__rmc_170,__rmc_169,__rmc_168,__rmc_167,__rmc_166,__rmc_165,__rmc_164,__rmc_163,__rmc_162,__rmc_161 \
	,__rmc_160,__rmc_159,__rmc_158,__rmc_157,__rmc_156,__rmc_155,__rmc_154,__rmc_153,__rmc_152,__rmc_151,__rmc_150,__rmc_149,__rmc_148,__rmc_147,__rmc_146,__rmc_145 \
	,__rmc_144,__rmc_143,__rmc_142,__rmc_141,__rmc_140,__rmc_139,__rmc_138,__rmc_137,__rmc_136,__rmc_135,__rmc_134,__rmc_133,__rmc_132,__rmc_131,__rmc_130,__rmc_129 \
	,__rmc_128,__rmc_127,__rmc_126,__rmc_125,__rmc_124,__rmc_123,__rmc_122,__rmc_121,__rmc_120,__rmc_119,__rmc_118,__rmc_117,__rmc_116,__rmc_115,__rmc_114,__rmc_113 \
	,__rmc_112,__rmc_111,__rmc_110,__rmc_109,__rmc_108,__rmc_107,__rmc_106,__rmc_105,__rmc_104,__rmc_103,__rmc_102,__rmc_101,__rmc_100,__rmc_99,__rmc_98,__rmc_97 \
	,__rmc_96,__rmc_95,__rmc_94,__rmc_93,__rmc_92,__rmc_91,__rmc_90,__rmc_89,__rmc_88,__rmc_87,__rmc_86,__rmc_85,__rmc_84,__rmc_83,__rmc_82,__rmc_81 \
	,__rmc_80,__rmc_79,__rmc_78,__rmc_77,__rmc_76,__rmc_75,__rmc_74,__rmc_73,__rmc_72,__rmc_71,__rmc_70,__rmc_69,__rmc_68,__rmc_67,__rmc_66,__rmc_65 \
	,__rmc_64,__rmc_63,__rmc_62,__rmc_61,__rmc_60,__rmc_59,__rmc_58,__rmc_57,__rmc_56,__rmc_55,__rmc_54,__rmc_53,__rmc_52,__rmc_51,__rmc_50,__rmc_49 \
	,__rmc_48,__rmc_47,__rmc_46,__rmc_45,__rmc_44,__rmc_43,__rmc_42,__rmc_41,__rmc_40,__rmc_39,__rmc_38,__rmc_37,__rmc_36,__rmc_35,__rmc_34,__rmc_33 \
	,__rmc_32,__rmc_31,__rmc_30,__rmc_29,__rmc_28,__rmc_27,__rmc_26,__rmc_25,__rmc_24,__rmc_23,__rmc_22,__rmc_21,__rmc_20,__rmc_19,__rmc_18,__rmc_17 \
	,__rmc_16,__rmc_15,__rmc_14,__rmc_13,__rmc_12,__rmc_11,__rmc_10,__rmc_9,__rmc_8,__rmc_7,__rmc_6,__rmc_5,__rmc_4,__rmc_3,__rmc_2,__rmc_1 \
	,__rmc_0)(x, ##__VA_ARGS__)

/************************************************************************************
emulate recursive macro calls
************************************************************************************/

#define __rmc_0(_call, ...)
#define __rmc_1(_call, x) _call(x)
#define __rmc_2(_call, x, ...) _call(x) __rmc_1(_call, __VA_ARGS__)
#define __rmc_3(_call, x, ...) _call(x) __rmc_2(_call, __VA_ARGS__)
#define __rmc_4(_call, x, ...) _call(x) __rmc_3(_call, __VA_ARGS__)
#define __rmc_5(_call, x, ...) _call(x) __rmc_4(_call, __VA_ARGS__)
#define __rmc_6(_call, x, ...) _call(x) __rmc_5(_call, __VA_ARGS__)
#define __rmc_7(_call, x, ...) _call(x) __rmc_6(_call, __VA_ARGS__)
#define __rmc_8(_call, x, ...) _call(x) __rmc_7(_call, __VA_ARGS__)
#define __rmc_9(_call, x, ...) _call(x) __rmc_8(_call, __VA_ARGS__)
#define __rmc_10(_call, x, ...) _call(x) __rmc_9(_call, __VA_ARGS__)
#define __rmc_11(_call, x, ...) _call(x) __rmc_10(_call, __VA_ARGS__)
#define __rmc_12(_call, x, ...) _call(x) __rmc_11(_call, __VA_ARGS__)
#define __rmc_13(_call, x, ...) _call(x) __rmc_12(_call, __VA_ARGS__)
#define __rmc_14(_call, x, ...) _call(x) __rmc_13(_call, __VA_ARGS__)
#define __rmc_15(_call, x, ...) _call(x) __rmc_14(_call, __VA_ARGS__)
#define __rmc_16(_call, x, ...) _call(x) __rmc_15(_call, __VA_ARGS__)
#define __rmc_17(_call, x, ...) _call(x) __rmc_16(_call, __VA_ARGS__)
#define __rmc_18(_call, x, ...) _call(x) __rmc_17(_call, __VA_ARGS__)
#define __rmc_19(_call, x, ...) _call(x) __rmc_18(_call, __VA_ARGS__)
#define __rmc_20(_call, x, ...) _call(x) __rmc_19(_call, __VA_ARGS__)
#define __rmc_21(_call, x, ...) _call(x) __rmc_20(_call, __VA_ARGS__)
#define __rmc_22(_call, x, ...) _call(x) __rmc_21(_call, __VA_ARGS__)
#define __rmc_23(_call, x, ...) _call(x) __rmc_22(_call, __VA_ARGS__)
#define __rmc_24(_call, x, ...) _call(x) __rmc_23(_call, __VA_ARGS__)
#define __rmc_25(_call, x, ...) _call(x) __rmc_24(_call, __VA_ARGS__)
#define __rmc_26(_call, x, ...) _call(x) __rmc_25(_call, __VA_ARGS__)
#define __rmc_27(_call, x, ...) _call(x) __rmc_26(_call, __VA_ARGS__)
#define __rmc_28(_call, x, ...) _call(x) __rmc_27(_call, __VA_ARGS__)
#define __rmc_29(_call, x, ...) _call(x) __rmc_28(_call, __VA_ARGS__)
#define __rmc_30(_call, x, ...) _call(x) __rmc_29(_call, __VA_ARGS__)
#define __rmc_31(_call, x, ...) _call(x) __rmc_30(_call, __VA_ARGS__)
#define __rmc_32(_call, x, ...) _call(x) __rmc_31(_call, __VA_ARGS__)
#define __rmc_33(_call, x, ...) _call(x) __rmc_32(_call, __VA_ARGS__)
#define __rmc_34(_call, x, ...) _call(x) __rmc_33(_call, __VA_ARGS__)
#define __rmc_35(_call, x, ...) _call(x) __rmc_34(_call, __VA_ARGS__)
#define __rmc_36(_call, x, ...) _call(x) __rmc_35(_call, __VA_ARGS__)
#define __rmc_37(_call, x, ...) _call(x) __rmc_36(_call, __VA_ARGS__)
#define __rmc_38(_call, x, ...) _call(x) __rmc_37(_call, __VA_ARGS__)
#define __rmc_39(_call, x, ...) _call(x) __rmc_38(_call, __VA_ARGS__)
#define __rmc_40(_call, x, ...) _call(x) __rmc_39(_call, __VA_ARGS__)
#define __rmc_41(_call, x, ...) _call(x) __rmc_40(_call, __VA_ARGS__)
#define __rmc_42(_call, x, ...) _call(x) __rmc_41(_call, __VA_ARGS__)
#define __rmc_43(_call, x, ...) _call(x) __rmc_42(_call, __VA_ARGS__)
#define __rmc_44(_call, x, ...) _call(x) __rmc_43(_call, __VA_ARGS__)
#define __rmc_45(_call, x, ...) _call(x) __rmc_44(_call, __VA_ARGS__)
#define __rmc_46(_call, x, ...) _call(x) __rmc_45(_call, __VA_ARGS__)
#define __rmc_47(_call, x, ...) _call(x) __rmc_46(_call, __VA_ARGS__)
#define __rmc_48(_call, x, ...) _call(x) __rmc_47(_call, __VA_ARGS__)
#define __rmc_49(_call, x, ...) _call(x) __rmc_48(_call, __VA_ARGS__)
#define __rmc_50(_call, x, ...) _call(x) __rmc_49(_call, __VA_ARGS__)
#define __rmc_51(_call, x, ...) _call(x) __rmc_50(_call, __VA_ARGS__)
#define __rmc_52(_call, x, ...) _call(x) __rmc_51(_call, __VA_ARGS__)
#define __rmc_53(_call, x, ...) _call(x) __rmc_52(_call, __VA_ARGS__)
#define __rmc_54(_call, x, ...) _call(x) __rmc_53(_call, __VA_ARGS__)
#define __rmc_55(_call, x, ...) _call(x) __rmc_54(_call, __VA_ARGS__)
#define __rmc_56(_call, x, ...) _call(x) __rmc_55(_call, __VA_ARGS__)
#define __rmc_57(_call, x, ...) _call(x) __rmc_56(_call, __VA_ARGS__)
#define __rmc_58(_call, x, ...) _call(x) __rmc_57(_call, __VA_ARGS__)
#define __rmc_59(_call, x, ...) _call(x) __rmc_58(_call, __VA_ARGS__)
#define __rmc_60(_call, x, ...) _call(x) __rmc_59(_call, __VA_ARGS__)
#define __rmc_61(_call, x, ...) _call(x) __rmc_60(_call, __VA_ARGS__)
#define __rmc_62(_call, x, ...) _call(x) __rmc_61(_call, __VA_ARGS__)
#define __rmc_63(_call, x, ...) _call(x) __rmc_62(_call, __VA_ARGS__)
#define __rmc_64(_call, x, ...) _call(x) __rmc_63(_call, __VA_ARGS__)
#define __rmc_65(_call, x, ...) _call(x) __rmc_64(_call, __VA_ARGS__)
#define __rmc_66(_call, x, ...) _call(x) __rmc_65(_call, __VA_ARGS__)
#define __rmc_67(_call, x, ...) _call(x) __rmc_66(_call, __VA_ARGS__)
#define __rmc_68(_call, x, ...) _call(x) __rmc_67(_call, __VA_ARGS__)
#define __rmc_69(_call, x, ...) _call(x) __rmc_68(_call, __VA_ARGS__)
#define __rmc_70(_call, x, ...) _call(x) __rmc_69(_call, __VA_ARGS__)
#define __rmc_71(_call, x, ...) _call(x) __rmc_70(_call, __VA_ARGS__)
#define __rmc_72(_call, x, ...) _call(x) __rmc_71(_call, __VA_ARGS__)
#define __rmc_73(_call, x, ...) _call(x) __rmc_72(_call, __VA_ARGS__)
#define __rmc_74(_call, x, ...) _call(x) __rmc_73(_call, __VA_ARGS__)
#define __rmc_75(_call, x, ...) _call(x) __rmc_74(_call, __VA_ARGS__)
#define __rmc_76(_call, x, ...) _call(x) __rmc_75(_call, __VA_ARGS__)
#define __rmc_77(_call, x, ...) _call(x) __rmc_76(_call, __VA_ARGS__)
#define __rmc_78(_call, x, ...) _call(x) __rmc_77(_call, __VA_ARGS__)
#define __rmc_79(_call, x, ...) _call(x) __rmc_78(_call, __VA_ARGS__)
#define __rmc_80(_call, x, ...) _call(x) __rmc_79(_call, __VA_ARGS__)
#define __rmc_81(_call, x, ...) _call(x) __rmc_80(_call, __VA_ARGS__)
#define __rmc_82(_call, x, ...) _call(x) __rmc_81(_call, __VA_ARGS__)
#define __rmc_83(_call, x, ...) _call(x) __rmc_82(_call, __VA_ARGS__)
#define __rmc_84(_call, x, ...) _call(x) __rmc_83(_call, __VA_ARGS__)
#define __rmc_85(_call, x, ...) _call(x) __rmc_84(_call, __VA_ARGS__)
#define __rmc_86(_call, x, ...) _call(x) __rmc_85(_call, __VA_ARGS__)
#define __rmc_87(_call, x, ...) _call(x) __rmc_86(_call, __VA_ARGS__)
#define __rmc_88(_call, x, ...) _call(x) __rmc_87(_call, __VA_ARGS__)
#define __rmc_89(_call, x, ...) _call(x) __rmc_88(_call, __VA_ARGS__)
#define __rmc_90(_call, x, ...) _call(x) __rmc_89(_call, __VA_ARGS__)
#define __rmc_91(_call, x, ...) _call(x) __rmc_90(_call, __VA_ARGS__)
#define __rmc_92(_call, x, ...) _call(x) __rmc_91(_call, __VA_ARGS__)
#define __rmc_93(_call, x, ...) _call(x) __rmc_92(_call, __VA_ARGS__)
#define __rmc_94(_call, x, ...) _call(x) __rmc_93(_call, __VA_ARGS__)
#define __rmc_95(_call, x, ...) _call(x) __rmc_94(_call, __VA_ARGS__)
#define __rmc_96(_call, x, ...) _call(x) __rmc_95(_call, __VA_ARGS__)
#define __rmc_97(_call, x, ...) _call(x) __rmc_96(_call, __VA_ARGS__)
#define __rmc_98(_call, x, ...) _call(x) __rmc_97(_call, __VA_ARGS__)
#define __rmc_99(_call, x, ...) _call(x) __rmc_98(_call, __VA_ARGS__)
#define __rmc_100(_call, x, ...) _call(x) __rmc_99(_call, __VA_ARGS__)
#define __rmc_101(_call, x, ...) _call(x) __rmc_100(_call, __VA_ARGS__)
#define __rmc_102(_call, x, ...) _call(x) __rmc_101(_call, __VA_ARGS__)
#define __rmc_103(_call, x, ...) _call(x) __rmc_102(_call, __VA_ARGS__)
#define __rmc_104(_call, x, ...) _call(x) __rmc_103(_call, __VA_ARGS__)
#define __rmc_105(_call, x, ...) _call(x) __rmc_104(_call, __VA_ARGS__)
#define __rmc_106(_call, x, ...) _call(x) __rmc_105(_call, __VA_ARGS__)
#define __rmc_107(_call, x, ...) _call(x) __rmc_106(_call, __VA_ARGS__)
#define __rmc_108(_call, x, ...) _call(x) __rmc_107(_call, __VA_ARGS__)
#define __rmc_109(_call, x, ...) _call(x) __rmc_108(_call, __VA_ARGS__)
#define __rmc_110(_call, x, ...) _call(x) __rmc_109(_call, __VA_ARGS__)
#define __rmc_111(_call, x, ...) _call(x) __rmc_110(_call, __VA_ARGS__)
#define __rmc_112(_call, x, ...) _call(x) __rmc_111(_call, __VA_ARGS__)
#define __rmc_113(_call, x, ...) _call(x) __rmc_112(_call, __VA_ARGS__)
#define __rmc_114(_call, x, ...) _call(x) __rmc_113(_call, __VA_ARGS__)
#define __rmc_115(_call, x, ...) _call(x) __rmc_114(_call, __VA_ARGS__)
#define __rmc_116(_call, x, ...) _call(x) __rmc_115(_call, __VA_ARGS__)
#define __rmc_117(_call, x, ...) _call(x) __rmc_116(_call, __VA_ARGS__)
#define __rmc_118(_call, x, ...) _call(x) __rmc_117(_call, __VA_ARGS__)
#define __rmc_119(_call, x, ...) _call(x) __rmc_118(_call, __VA_ARGS__)
#define __rmc_120(_call, x, ...) _call(x) __rmc_119(_call, __VA_ARGS__)
#define __rmc_121(_call, x, ...) _call(x) __rmc_120(_call, __VA_ARGS__)
#define __rmc_122(_call, x, ...) _call(x) __rmc_121(_call, __VA_ARGS__)
#define __rmc_123(_call, x, ...) _call(x) __rmc_122(_call, __VA_ARGS__)
#define __rmc_124(_call, x, ...) _call(x) __rmc_123(_call, __VA_ARGS__)
#define __rmc_125(_call, x, ...) _call(x) __rmc_124(_call, __VA_ARGS__)
#define __rmc_126(_call, x, ...) _call(x) __rmc_125(_call, __VA_ARGS__)
#define __rmc_127(_call, x, ...) _call(x) __rmc_126(_call, __VA_ARGS__)
#define __rmc_128(_call, x, ...) _call(x) __rmc_127(_call, __VA_ARGS__)
#define __rmc_129(_call, x, ...) _call(x) __rmc_128(_call, __VA_ARGS__)
#define __rmc_130(_call, x, ...) _call(x) __rmc_129(_call, __VA_ARGS__)
#define __rmc_131(_call, x, ...) _call(x) __rmc_130(_call, __VA_ARGS__)
#define __rmc_132(_call, x, ...) _call(x) __rmc_131(_call, __VA_ARGS__)
#define __rmc_133(_call, x, ...) _call(x) __rmc_132(_call, __VA_ARGS__)
#define __rmc_134(_call, x, ...) _call(x) __rmc_133(_call, __VA_ARGS__)
#define __rmc_135(_call, x, ...) _call(x) __rmc_134(_call, __VA_ARGS__)
#define __rmc_136(_call, x, ...) _call(x) __rmc_135(_call, __VA_ARGS__)
#define __rmc_137(_call, x, ...) _call(x) __rmc_136(_call, __VA_ARGS__)
#define __rmc_138(_call, x, ...) _call(x) __rmc_137(_call, __VA_ARGS__)
#define __rmc_139(_call, x, ...) _call(x) __rmc_138(_call, __VA_ARGS__)
#define __rmc_140(_call, x, ...) _call(x) __rmc_139(_call, __VA_ARGS__)
#define __rmc_141(_call, x, ...) _call(x) __rmc_140(_call, __VA_ARGS__)
#define __rmc_142(_call, x, ...) _call(x) __rmc_141(_call, __VA_ARGS__)
#define __rmc_143(_call, x, ...) _call(x) __rmc_142(_call, __VA_ARGS__)
#define __rmc_144(_call, x, ...) _call(x) __rmc_143(_call, __VA_ARGS__)
#define __rmc_145(_call, x, ...) _call(x) __rmc_144(_call, __VA_ARGS__)
#define __rmc_146(_call, x, ...) _call(x) __rmc_145(_call, __VA_ARGS__)
#define __rmc_147(_call, x, ...) _call(x) __rmc_146(_call, __VA_ARGS__)
#define __rmc_148(_call, x, ...) _call(x) __rmc_147(_call, __VA_ARGS__)
#define __rmc_149(_call, x, ...) _call(x) __rmc_148(_call, __VA_ARGS__)
#define __rmc_150(_call, x, ...) _call(x) __rmc_149(_call, __VA_ARGS__)
#define __rmc_151(_call, x, ...) _call(x) __rmc_150(_call, __VA_ARGS__)
#define __rmc_152(_call, x, ...) _call(x) __rmc_151(_call, __VA_ARGS__)
#define __rmc_153(_call, x, ...) _call(x) __rmc_152(_call, __VA_ARGS__)
#define __rmc_154(_call, x, ...) _call(x) __rmc_153(_call, __VA_ARGS__)
#define __rmc_155(_call, x, ...) _call(x) __rmc_154(_call, __VA_ARGS__)
#define __rmc_156(_call, x, ...) _call(x) __rmc_155(_call, __VA_ARGS__)
#define __rmc_157(_call, x, ...) _call(x) __rmc_156(_call, __VA_ARGS__)
#define __rmc_158(_call, x, ...) _call(x) __rmc_157(_call, __VA_ARGS__)
#define __rmc_159(_call, x, ...) _call(x) __rmc_158(_call, __VA_ARGS__)
#define __rmc_160(_call, x, ...) _call(x) __rmc_159(_call, __VA_ARGS__)
#define __rmc_161(_call, x, ...) _call(x) __rmc_160(_call, __VA_ARGS__)
#define __rmc_162(_call, x, ...) _call(x) __rmc_161(_call, __VA_ARGS__)
#define __rmc_163(_call, x, ...) _call(x) __rmc_162(_call, __VA_ARGS__)
#define __rmc_164(_call, x, ...) _call(x) __rmc_163(_call, __VA_ARGS__)
#define __rmc_165(_call, x, ...) _call(x) __rmc_164(_call, __VA_ARGS__)
#define __rmc_166(_call, x, ...) _call(x) __rmc_165(_call, __VA_ARGS__)
#define __rmc_167(_call, x, ...) _call(x) __rmc_166(_call, __VA_ARGS__)
#define __rmc_168(_call, x, ...) _call(x) __rmc_167(_call, __VA_ARGS__)
#define __rmc_169(_call, x, ...) _call(x) __rmc_168(_call, __VA_ARGS__)
#define __rmc_170(_call, x, ...) _call(x) __rmc_169(_call, __VA_ARGS__)
#define __rmc_171(_call, x, ...) _call(x) __rmc_170(_call, __VA_ARGS__)
#define __rmc_172(_call, x, ...) _call(x) __rmc_171(_call, __VA_ARGS__)
#define __rmc_173(_call, x, ...) _call(x) __rmc_172(_call, __VA_ARGS__)
#define __rmc_174(_call, x, ...) _call(x) __rmc_173(_call, __VA_ARGS__)
#define __rmc_175(_call, x, ...) _call(x) __rmc_174(_call, __VA_ARGS__)
#define __rmc_176(_call, x, ...) _call(x) __rmc_175(_call, __VA_ARGS__)
#define __rmc_177(_call, x, ...) _call(x) __rmc_176(_call, __VA_ARGS__)
#define __rmc_178(_call, x, ...) _call(x) __rmc_177(_call, __VA_ARGS__)
#define __rmc_179(_call, x, ...) _call(x) __rmc_178(_call, __VA_ARGS__)
#define __rmc_180(_call, x, ...) _call(x) __rmc_179(_call, __VA_ARGS__)
#define __rmc_181(_call, x, ...) _call(x) __rmc_180(_call, __VA_ARGS__)
#define __rmc_182(_call, x, ...) _call(x) __rmc_181(_call, __VA_ARGS__)
#define __rmc_183(_call, x, ...) _call(x) __rmc_182(_call, __VA_ARGS__)
#define __rmc_184(_call, x, ...) _call(x) __rmc_183(_call, __VA_ARGS__)
#define __rmc_185(_call, x, ...) _call(x) __rmc_184(_call, __VA_ARGS__)
#define __rmc_186(_call, x, ...) _call(x) __rmc_185(_call, __VA_ARGS__)
#define __rmc_187(_call, x, ...) _call(x) __rmc_186(_call, __VA_ARGS__)
#define __rmc_188(_call, x, ...) _call(x) __rmc_187(_call, __VA_ARGS__)
#define __rmc_189(_call, x, ...) _call(x) __rmc_188(_call, __VA_ARGS__)
#define __rmc_190(_call, x, ...) _call(x) __rmc_189(_call, __VA_ARGS__)
#define __rmc_191(_call, x, ...) _call(x) __rmc_190(_call, __VA_ARGS__)
#define __rmc_192(_call, x, ...) _call(x) __rmc_191(_call, __VA_ARGS__)
#define __rmc_193(_call, x, ...) _call(x) __rmc_192(_call, __VA_ARGS__)
#define __rmc_194(_call, x, ...) _call(x) __rmc_193(_call, __VA_ARGS__)
#define __rmc_195(_call, x, ...) _call(x) __rmc_194(_call, __VA_ARGS__)
#define __rmc_196(_call, x, ...) _call(x) __rmc_195(_call, __VA_ARGS__)
#define __rmc_197(_call, x, ...) _call(x) __rmc_196(_call, __VA_ARGS__)
#define __rmc_198(_call, x, ...) _call(x) __rmc_197(_call, __VA_ARGS__)
#define __rmc_199(_call, x, ...) _call(x) __rmc_198(_call, __VA_ARGS__)
#define __rmc_200(_call, x, ...) _call(x) __rmc_199(_call, __VA_ARGS__)
#define __rmc_201(_call, x, ...) _call(x) __rmc_200(_call, __VA_ARGS__)
#define __rmc_202(_call, x, ...) _call(x) __rmc_201(_call, __VA_ARGS__)
#define __rmc_203(_call, x, ...) _call(x) __rmc_202(_call, __VA_ARGS__)
#define __rmc_204(_call, x, ...) _call(x) __rmc_203(_call, __VA_ARGS__)
#define __rmc_205(_call, x, ...) _call(x) __rmc_204(_call, __VA_ARGS__)
#define __rmc_206(_call, x, ...) _call(x) __rmc_205(_call, __VA_ARGS__)
#define __rmc_207(_call, x, ...) _call(x) __rmc_206(_call, __VA_ARGS__)
#define __rmc_208(_call, x, ...) _call(x) __rmc_207(_call, __VA_ARGS__)
#define __rmc_209(_call, x, ...) _call(x) __rmc_208(_call, __VA_ARGS__)
#define __rmc_210(_call, x, ...) _call(x) __rmc_209(_call, __VA_ARGS__)
#define __rmc_211(_call, x, ...) _call(x) __rmc_210(_call, __VA_ARGS__)
#define __rmc_212(_call, x, ...) _call(x) __rmc_211(_call, __VA_ARGS__)
#define __rmc_213(_call, x, ...) _call(x) __rmc_212(_call, __VA_ARGS__)
#define __rmc_214(_call, x, ...) _call(x) __rmc_213(_call, __VA_ARGS__)
#define __rmc_215(_call, x, ...) _call(x) __rmc_214(_call, __VA_ARGS__)
#define __rmc_216(_call, x, ...) _call(x) __rmc_215(_call, __VA_ARGS__)
#define __rmc_217(_call, x, ...) _call(x) __rmc_216(_call, __VA_ARGS__)
#define __rmc_218(_call, x, ...) _call(x) __rmc_217(_call, __VA_ARGS__)
#define __rmc_219(_call, x, ...) _call(x) __rmc_218(_call, __VA_ARGS__)
#define __rmc_220(_call, x, ...) _call(x) __rmc_219(_call, __VA_ARGS__)
#define __rmc_221(_call, x, ...) _call(x) __rmc_220(_call, __VA_ARGS__)
#define __rmc_222(_call, x, ...) _call(x) __rmc_221(_call, __VA_ARGS__)
#define __rmc_223(_call, x, ...) _call(x) __rmc_222(_call, __VA_ARGS__)
#define __rmc_224(_call, x, ...) _call(x) __rmc_223(_call, __VA_ARGS__)
#define __rmc_225(_call, x, ...) _call(x) __rmc_224(_call, __VA_ARGS__)
#define __rmc_226(_call, x, ...) _call(x) __rmc_225(_call, __VA_ARGS__)
#define __rmc_227(_call, x, ...) _call(x) __rmc_226(_call, __VA_ARGS__)
#define __rmc_228(_call, x, ...) _call(x) __rmc_227(_call, __VA_ARGS__)
#define __rmc_229(_call, x, ...) _call(x) __rmc_228(_call, __VA_ARGS__)
#define __rmc_230(_call, x, ...) _call(x) __rmc_229(_call, __VA_ARGS__)
#define __rmc_231(_call, x, ...) _call(x) __rmc_230(_call, __VA_ARGS__)
#define __rmc_232(_call, x, ...) _call(x) __rmc_231(_call, __VA_ARGS__)
#define __rmc_233(_call, x, ...) _call(x) __rmc_232(_call, __VA_ARGS__)
#define __rmc_234(_call, x, ...) _call(x) __rmc_233(_call, __VA_ARGS__)
#define __rmc_235(_call, x, ...) _call(x) __rmc_234(_call, __VA_ARGS__)
#define __rmc_236(_call, x, ...) _call(x) __rmc_235(_call, __VA_ARGS__)
#define __rmc_237(_call, x, ...) _call(x) __rmc_236(_call, __VA_ARGS__)
#define __rmc_238(_call, x, ...) _call(x) __rmc_237(_call, __VA_ARGS__)
#define __rmc_239(_call, x, ...) _call(x) __rmc_238(_call, __VA_ARGS__)
#define __rmc_240(_call, x, ...) _call(x) __rmc_239(_call, __VA_ARGS__)
#define __rmc_241(_call, x, ...) _call(x) __rmc_240(_call, __VA_ARGS__)
#define __rmc_242(_call, x, ...) _call(x) __rmc_241(_call, __VA_ARGS__)
#define __rmc_243(_call, x, ...) _call(x) __rmc_242(_call, __VA_ARGS__)
#define __rmc_244(_call, x, ...) _call(x) __rmc_243(_call, __VA_ARGS__)
#define __rmc_245(_call, x, ...) _call(x) __rmc_244(_call, __VA_ARGS__)
#define __rmc_246(_call, x, ...) _call(x) __rmc_245(_call, __VA_ARGS__)
#define __rmc_247(_call, x, ...) _call(x) __rmc_246(_call, __VA_ARGS__)
#define __rmc_248(_call, x, ...) _call(x) __rmc_247(_call, __VA_ARGS__)
#define __rmc_249(_call, x, ...) _call(x) __rmc_248(_call, __VA_ARGS__)
#define __rmc_250(_call, x, ...) _call(x) __rmc_249(_call, __VA_ARGS__)
#define __rmc_251(_call, x, ...) _call(x) __rmc_250(_call, __VA_ARGS__)
#define __rmc_252(_call, x, ...) _call(x) __rmc_251(_call, __VA_ARGS__)
#define __rmc_253(_call, x, ...) _call(x) __rmc_252(_call, __VA_ARGS__)
#define __rmc_254(_call, x, ...) _call(x) __rmc_253(_call, __VA_ARGS__)


#endif // USPOOKYMACROS_h

