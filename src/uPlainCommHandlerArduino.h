#ifndef UPLAINCOMMHANDLERARDUINO_H
#define UPLAINCOMMHANDLERARDUINO_H

#include "uPlainCommHandler.h"

class TplainCommHandler : public TplainCommHandlerTemplate<Stream,String>{
  using TplainCommHandlerTemplate::TplainCommHandlerTemplate;
};

#endif