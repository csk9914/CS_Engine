#pragma once
#include <cstdint>
#include <string>
#include "NameGenerator.h"


class Object
{
public:
	Object(const std::string& name = "New Object", bool useIndexing = false) : m_id(GenerateID()), m_active(true)
	{
		SetName(name, useIndexing);
	}

	virtual ~Object() = default;

	// 공통 기능
	uint64_t GetID() const { return m_id; }


	void SetName(const std::string& name, bool useIndexing = false)
	{
			m_name = useIndexing ? NameGenerator::GenerateUniqueName(name) : name;
	}

	const std::string& GetName() const { return m_name; }

public:
	// 활성화
	bool GetActive() const { return m_active; }
	virtual void SetActive(bool active) { m_active = active; }

private:
	uint64_t m_id;
	std::string m_name;
	bool m_active;

	static uint64_t GenerateID() { static uint64_t nextID = 0; return nextID++; }
};