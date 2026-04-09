#pragma once

#include <vector>

// vector 두 개를 스왑하는 방식
template<typename T>
class CSQueue
{
public:
	void push(const T& val)
	{
		m_wait.emplace_back(val);
	}

	std::vector<T>& prepare()
	{
		m_work.clear();
		m_work.swap(m_wait);

		return m_work;
	}

	bool empty() const { return m_wait.empty(); }

	void clear()
	{
		m_wait.clear();
		m_work.clear();
	}

private:
	std::vector<T> m_wait;
	std::vector<T> m_work;

};