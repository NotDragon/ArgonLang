#include <utility>
#include "Error/Result.h"

ArgonLang::Trace::Trace(): text(""), type(), position({ 0, 0 }) { }
ArgonLang::Trace::Trace(std::string  text, ASTNodeType type, Token::Position position): text(std::move(text)), type(type), position(position) {}