/*
 * MacroQuest2: The extension platform for EverQuest
 * Copyright (C) 2002-2019 MacroQuest Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "pch.h"
#include "MQ2DataTypes.h"

using namespace mq;
using namespace mq::datatypes;

static bool IsFalsey(std::string_view var)
{
	return var.empty()
		|| ci_equals(var, "FALSE")
		|| ci_equals(var, "NULL")
		|| GetFloatFromString(var, 1) == 0;
}

// pure type, no members
bool MQ2BoolType::GetMember(MQVarPtr VarPtr, const char* Member, char* Index, MQTypeVar& Dest)
{
	return false;
}

bool MQ2BoolType::ToString(MQVarPtr VarPtr, char* Destination)
{
	strcpy_s(Destination, MAX_STRING, VarPtr.Get<bool>() ? "TRUE" : "FALSE");
	return true;
}

bool MQ2BoolType::FromData(MQVarPtr& VarPtr, MQTypeVar& Source)
{
	VarPtr.Set(Source.Get<bool>());
	return true;
}

bool MQ2BoolType::FromString(MQVarPtr& VarPtr, const char* Source)
{
	VarPtr.Set(!IsFalsey(Source));
	return true;
}

bool MQ2BoolType::dataBool(const char* szIndex, MQTypeVar& Ret)
{
	if (!szIndex[0])
		return false;

	Ret.Type = pBoolType;
	Ret.Set(!IsFalsey(szIndex));
	return true;
}
