#pragma once
#include <memory>
#include <brick-types>
#include "ast.h"

using brick::types::Union;

using Value = Union<std::string, int>;

//TODO
struct ExecutionStack {
	struct Frame {

	};
};