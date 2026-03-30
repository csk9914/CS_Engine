#pragma once
#include "IUpdate.h"
#include "IRender.h"
#include "IStart.h"
#include "IEnd.h"

class IGame : public IUpdate, public IRender, public IStart, public IEnd { };