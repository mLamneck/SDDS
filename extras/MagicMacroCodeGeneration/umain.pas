unit uMain;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, Forms, Controls, Graphics, Dialogs, StdCtrls;

type

  { TForm1 }

  TForm1 = class(TForm)
    bGenerate: TButton;
    eMaxParams: TEdit;
    eFilename: TEdit;
    GroupBox1: TGroupBox;
    GroupBox2: TGroupBox;
    lbResult: TMemo;
    procedure bGenerateClick(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private

  public

  end;

var
  Form1: TForm1;

implementation

{$R *.lfm}

{ TForm1 }



procedure TForm1.bGenerateClick(Sender: TObject);
var list : TstringList; n,i : integer; line : string;
var GET_NTH_NAME : string;
const
  FE_NAME = '__rmc_';
  cGET_NTH_NAME = '__SP_GET_ARG_%d';
  COUNT_ARG_NAME = 'SP_COUNT_VARARGS';
  FOR_EACH_NAME = 'SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM';
begin
  n := strToInt(eMaxParams.text);
  GET_NTH_NAME := format(cGET_NTH_NAME,[n+2]);
  list := TstringList.create();

  //add inlcude guard first
  list.add('#ifndef USPOOKYMACROS_h');
  list.add('#define USPOOKYMACROS_h');
  list.add('');

  list.add('/************************************************************************************');
  list.add('uSpookyMacros.h');
  list.add('');
  list.add(format('Auto generated file by C++ Spooky Macro Generator',[]));
  list.add(format(#9'Author: Mark Lamneck',[]));
  list.add(format(#9'Creation Date: %s',[formatDateTime('dd.mm.YY',now())]));
  list.add(format(#9'macros with preceding __ are for internal use',[]));
  list.add('');
  list.add('Macros to be used:');
  list.add(#9'I. %s(...):',[COUNT_ARG_NAME]);
  list.add(#9#9'macro to count the number of arguments given to a variadic macro (up to %d)',[n]);
  list.add(#9#9'%s(A,B,C) -> 3',[COUNT_ARG_NAME]);
  list.add(#9#9'%s(5,4,3,2,1) -> 5',[COUNT_ARG_NAME]);

  list.add('');
  list.add(#9'II. %s(x,...):',[FOR_EACH_NAME]);
  list.add(#9#9'call macro x for all params in ... (for up to %d arguments) like: ',[n]);
  list.add(#9#9'for param in ... do x(param) where param is a single value',[n]);
  list.add(#9#9'example: given a macro: #define to_comma_sep_list(_a) _a,',[]);
  list.add(#9#9#9'%s(to_comma_sep_list, A, B, C) -> A,B,C,',[FOR_EACH_NAME]);
  list.add('************************************************************************************/');
  list.add('');

  n := n+1;  //we need one more
  //****************************************************************************
  //get nth arg + count parameter macro
  //****************************************************************************

  //#define __SP_GET_ARG_n(_1, _2, ...,_n, N, ...) N
  list.add(format('#define %s( \',[GET_NTH_NAME]));
  line := #9;
  for i := 1 to n do begin
    line := line + '_' + intToStr(i) + ',';
    if (i mod 32 = 0) then begin
       list.add(line + ' \');
       line := #9;
    end;
  end;
  line := line + 'N, ...) N';
  list.Add(line);

  //#define COUNT_VARARGS(...) __SP_GET_ARG_n("ignored", ##__VA_ARGS__, n-1, n-1, ..., 0)
  list.Add('');
  list.Add(format('#define %s(...) %s("ignored", ##__VA_ARGS__ \',[COUNT_ARG_NAME,GET_NTH_NAME]));
  line := #9;
  for i := n-1 downto 0 do begin
    if (i mod 32 = 0) then begin
       list.add(line + ' \');
       line := #9;
    end;
    line := line + ',' + intToStr(i);
  end;
  list.add(line + ')');

  {
  #define SP_FOR_EACH_PARAM_CALL_MACRO_WITH_PARAM(x, ...) \
    FE_GET_NTH_ARG("ignored", ##__VA_ARGS__, \
    _rmc_4, _rmc_3, _rmc_2, _rmc_1, _rmc_0)(x, ##__VA_ARGS__)
  }
  list.add('');
  list.add(format('#define %s(x, ...) %s("ignored", ##__VA_ARGS__ \',[FOR_EACH_NAME,GET_NTH_NAME]));
  line := '';
  for i := n-1 downto 0 do begin
      if (i mod 16 = 0) then begin
         list.add(#9 + line + ' \');
         line := '';
      end;
      line := line + ',' + FE_NAME + intToStr(i);
  end;
  list.add(#9 + line + ')(x, ##__VA_ARGS__)');
  {
  list.add(format('#define %s(x, ...) \',[FOR_EACH_NAME]));
  list.add(format('    %s("ignored", ##__VA_ARGS__, \',[GET_NTH_NAME]));
  line := format('    ' + FE_NAME + '%d',[n-1]);
  for i := n-2 downto 0 do begin
      line := line + ',' + FE_NAME + intToStr(i);
  end;
  list.add(line + ')(x, ##__VA_ARGS__)');
  }
  //****************************************************************************
  //helper macros for looping invocation
  //****************************************************************************

  list.add('');
  list.add('/************************************************************************************');
  list.add('emulate recursive macro calls ');
  list.add('************************************************************************************/');
  list.add('');

  //#define _rmc_0(_call, ...)
  //#define _rmc_1(_call, x) _call(x)
  list.Add('#define ' + FE_NAME + '0(_call, ...)');
  list.Add('#define ' + FE_NAME + '1(_call, x) _call(x)');

  //#define _fe_j(_call, x) _call(x) _fe_j-1(_call,__VA_ARGS__)
  for i := 2 to n-1 do begin
    list.add(format('#define ' + FE_NAME + '%d(_call, x, ...) _call(x) ' + FE_NAME + '%d(_call, __VA_ARGS__)',[i,i-1]));
  end;
  list.add('');

  //****************************************************************************
  //count parameters of variadic macros
  //****************************************************************************

  list.add('');
  list.add('#endif // USPOOKYMACROS_h');
  list.add('');
  list.SaveToFile(eFilename.text);
  lbResult.lines := list;
  list.free();

end;

procedure TForm1.FormCreate(Sender: TObject);
begin
  bGenerateClick(nil);
end;


end.

