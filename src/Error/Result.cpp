#include <utility>
#include "Error/Result.h"

ArgonLang::Trace::Trace(): type(), position({ 0, 0 }) { }


ArgonLang::Trace::Trace(ASTNodeType type, Token::Position position): type(type), position(position) {}