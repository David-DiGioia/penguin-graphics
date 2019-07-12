#pragma once

#include <string>
#include <chrono>
#include <set> // for std::set
#include <algorithm> // for std::upper_bound
#include "glm/glm.hpp"

#include "Core.h"
#include "MeshData.h"

namespace Util {

	class Timer
	{
	public:
		Timer();
		long long getMilliseconds();
		long long getNanoseconds();

	private:
		std::chrono::time_point<std::chrono::steady_clock> m_begin;
	};

	glm::mat4 createProjMatrix(const MeshData::FrustumData& fd);

	std::string stringFromFile(const char* filePath);

	std::vector<std::string> splitString(const std::string& s, const std::string& delim);

	MeshData::Mesh loadOBJ(const char* path);

	GLuint compileShader(GLenum type, const char* srcPath);

	inline float remap01(float begin, float end, float pos)
	{
		return (pos - begin) / (end - begin);
	}

	template<typename T>
	T linearlyInterpolate(const T& first, const T& second, float t)
	{
		return second * t + first * (1.0f - t);
	}

	template<typename K, typename V>
	struct KeyValue
	{
		K key;
		V value;

		KeyValue(const K& k, const V& v)
			: key{ k }, value{ v }
		{
		}

		inline bool operator< (const KeyValue& other) const { return key < other.key; }
		inline bool operator<= (const KeyValue& other) const { return key <= other.key; }
		inline bool operator> (const KeyValue& other) const { return !(key <= other.key); }
		inline bool operator>= (const KeyValue& other) const { return !(key < other.key); }
		inline bool operator== (const KeyValue& other) const { return !(key > other.key) && !(key < other.key); }
	};

	template<typename T>
	class Gradient
	{
	public:
		Gradient(T begin, T end)
			: values{ KeyValue<float, T>{0.0f, begin}, KeyValue<float, T>{1.0f, end} }
		{
		}

		Gradient(T beginEnd)
			: Gradient{beginEnd, beginEnd}
		{
		}

		// pos must be normalized
		void insert(float pos, T value)
		{
			values.insert(KeyValue<float, T>{pos, value});
		}

		T getLinear(float pos)
		{
			auto lower = std::lower_bound(values.begin(), values.end(), KeyValue<float, T>{pos, T{}});
			auto upper = std::upper_bound(values.begin(), values.end(), KeyValue<float, T>{pos, T{}});

			if (lower->key == pos)
				return lower->value;
			else
				--lower;

			const KeyValue<float, T>& prev = *lower;
			const KeyValue<float, T>& next = *upper;
			float t{ remap01(prev.key, next.key, pos) };
			return linearlyInterpolate(prev.value, next.value, t);
		}

	private:
		std::set<KeyValue<float, T>> values;
	};

}