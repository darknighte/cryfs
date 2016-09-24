#include "NoninteractiveConsole.h"

using std::string;
using std::vector;

namespace cpputils {

NoninteractiveConsole::NoninteractiveConsole(unique_ref<Console> baseConsole): _baseConsole(std::move(baseConsole)) {
}

bool NoninteractiveConsole::askYesNo(const string &/*question*/, bool defaultValue) {
    return defaultValue;
}

void NoninteractiveConsole::print(const std::string &output) {
    _baseConsole->print(output);
}

unsigned int NoninteractiveConsole::ask(const string &/*question*/, const vector<string> &/*options*/) {
    throw std::logic_error("Tried to ask a multiple choice question in noninteractive mode");
}

}
