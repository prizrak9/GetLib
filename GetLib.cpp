// GetLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include<iostream>
#include<Windows.h>

namespace MyInput {
	template<typename N>struct DeleteValue {
		DeleteValue(N&value) {}
	};
	template<typename N>struct DeleteValue<N*> {
		DeleteValue(N*&value) {
			if (value != nullptr) {
				delete value;
			}
		}
	};

	static class CheckType {
	public:
		template<typename T>
		static bool IsChar(T&value) {
			return false;
		}
		template<>
		static bool IsChar<char>(char&value) {
			return true;
		}
	};

	template<class T>
	class List {
		bool isEmpty;
		T value;
		List<T> *previous, *next;

		List(T&element, List&previous) {
			this->previous = &previous;
			value = element;
			next = nullptr;
			isEmpty = false;
		}
		void Add1(T&element) {
			next = new List(element, *this);
		}
		void Append(T&element) {
			if (isEmpty) {
				value = element;
				isEmpty = false;
			}
			else {
				Last().Add1(element);
			}
		}
		List<T>& Last() {
			List<T> *temp = this;
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			return *temp;
		}
	public:
		List() {
			isEmpty = true;
			previous = nullptr;
			next = nullptr;
		}
		~List() {
			DeleteValue<T> del(value);
		}

		void Add(T&element) {
			Append(element);
		}
		void Add(T*element) {
			Append(*element);
		}
		void Remove() {
			if (isEmpty)return;
			if (&Last() == this) {
				DeleteValue<T> del(value);
				isEmpty = true;
				return;
			}

			List<T> *temp = &Last();
			temp->previous->next = nullptr;
			delete temp;
		}
		void RemoveAll() {
			while (!isEmpty) {
				Remove();
			}
		}

		size_t Count() {
			if (isEmpty)return 0;
			size_t count = 1;
			List<T> *temp = this;
			while (temp->next != nullptr) {
				temp = temp->next;
				count++;
			}
			return count;
		}
		T& At(size_t index) {
			List<T> *temp = this;
			while (index-- > 0) {
				temp = temp->next;
			}
			return *(temp->value);
		}
		T& LastElement() {
			List<T> *temp = this;
			while (temp->next != nullptr) {
				temp = temp->next;
			}
			return (temp->value);
		}

		char* ToString() {
			if (!CheckType::IsChar(value))
				return "NO_CHAR";
			if (isEmpty)
				return "EMPTY";

			size_t len = Count(), i = 0;
			char* str = new char[len + 1];

			List<T> *temp = this;
			for (; i < len; i++) {
				str[i] = temp->value;
				temp = temp->next;
			}
			str[i] = '\0';

			return str;
		}
		bool TryParse(int&i) {
			if (isEmpty || !CheckType::IsChar(value))
				return false;
			char*temp = ToString();
			i = strtol(temp, NULL, 0);
			delete[]temp;
			return true;
		}
		bool TryParse(double&i) {
			if (isEmpty || !CheckType::IsChar(value))
				return false;
			char*temp = ToString();
			i = strtod(temp, NULL);
			delete[]temp;
			return true;
		}
	};

	class Settings {
	protected:
		const ptrdiff_t maxMaxLenght = 18;
	public:
		enum enterType {
			Undefined,
			String,
			StringPhoneNumber,
			Int,
			Double,
			DoublePrice
		}type = enterType::Undefined;
		enum endType {
			Stay,
			Return,
			NewLine
		}end = endType::Stay;

		size_t minLenght = 1;
		size_t maxLenght = maxMaxLenght;

		//for numbers
		bool HasValueLimit = false;
		ptrdiff_t minValue;
		ptrdiff_t maxValue;

		bool HasPointMaxNumberCountLimit = false;
		size_t afterPointMaxNumberCount;


		char*textForStartLine = nullptr;

		bool CanEscape = false;
	};
	class ProcessError {
		size_t Print(const char* text) {
			_cprintf(text);
			return strlen(text);
		}
	public:
		size_t TooLongFractional() {
			return Print(" (Fractional part has reached max length)");
		}
		size_t TooLong() {
			return Print(" (Max lenght was reached)");
		}
		size_t TooShort() {
			return Print(" (Value is too short)");
		}
		size_t NothingToErase() {
			return Print(" (Nothing to erase)");
		}
		size_t EnterSomething() {
			return Print(" (You must enter something)");
		}
		size_t WrongCharacter() {
			return Print(" (You can not enter such a character)");
		}
		size_t WrongValue() {
			return Print(" (Wrong value)");
		}
		size_t CanNotEscape() {
			return Print(" (You cannot skip entering this value)");
		}

	};

	class Get : public Settings, public ProcessError {
		COORD coord;
		const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		//short windowWidth;

		List<char> temp;
		bool isEscaped;

		bool point;
		size_t pointPos;

		bool IsFractionalTooLong(char c, size_t count) {
			return (point && HasPointMaxNumberCountLimit
				&& afterPointMaxNumberCount < (ptrdiff_t)count - pointPos);
		}

		bool IsString(char c) {
			return c > 32;
		}
		bool IsStringPhoneNumber(char c, size_t count) {
			return c > 47 && c < 58 || count == 0 && c == '+';
		}
		bool IsInt(char c, size_t count) {
			return c > 47 && c < 58 || count == 0 && c == '-';
		}
		bool IsDoublePrice(char c, size_t count) {
			if (IsFractionalTooLong(c, count)) {
				return false;
			}
			if (c == '.') {
				if (point) {
					return false;
				}
				else {
					pointPos = count;
					return point = true;
				}
			}
			else {
				return c > 47 && c < 58;
			}
		}
		bool IsDouble(char c, size_t count) {
			return IsDoublePrice(c, count) || count == 0 && c == '-';
		}

		bool IsAccaptable(char c, size_t count) {
			switch (type) {
			case enterType::String:
				return IsString(c);
			case enterType::StringPhoneNumber:
				return IsStringPhoneNumber(c, count);
			case enterType::Int:
				return IsInt(c, count);
			case enterType::Double:
				return IsDouble(c, count);
			case enterType::DoublePrice:
				return IsDoublePrice(c, count);
			default:
				return false;
			}
		}
		void Error(size_t(ProcessError::*error)() = nullptr) {
			static size_t errorLenght;
			if (error == nullptr && errorLenght != 0) {
				size_t i = 0;
				for (; i < errorLenght; i++)_putch(' ');
				for (; i > 0; i--)_putch('\b');
				errorLenght = 0;
			}
			else if (error != nullptr) {
				errorLenght = (*this.*error)();
				for (size_t i = errorLenght; i > 0; i--)_putch('\b');
			}
		}

		bool EndEntering(char c, size_t count) {
			if (count == 0) {
				Error(&ProcessError::EnterSomething);
				return false;
			}
			else if (count < minLenght) {
				Error(&ProcessError::TooShort);
				return false;
			}
			else {
				if (end == endType::NewLine) {
					_putch('\n');
				}
				else if (end == endType::Return) {
					SetConsoleCursorPosition(handle, coord);
				}

				return true;
			}
		}

		void Start() {
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
			coord = csbi.dwCursorPosition;
			//windowWidth = csbi.dwSize.X;

			point = false;
			isEscaped = false;
			GetValue();
		}
		void Finish() {
			type = enterType::Undefined;
			temp.RemoveAll();
		}

		void GetValue() {
			size_t count = temp.Count();
			char c = _getch();
			Error();

			if (IsAccaptable(c, count)) {
				if (count < maxLenght) {
					temp.Add(c);
					_putch(c);
					GetValue();
					return;
				}
				else {
					Error(&ProcessError::TooLong);
					GetValue();
					return;
				}
			}

			switch (c)
			{
			case '\r': {
				if (!EndEntering(c, count)) {
					GetValue();
				}
				return;
			}
			case '\b': {
				if (count == 0) {
					Error(&ProcessError::NothingToErase);
					GetValue();
				}
				else {
					_cprintf("\b \b");
					if (temp.LastElement() == '.') {
						point = false;
					}
					temp.Remove();
					GetValue();
				}
				return;
			}
			case '\033': {
				if (!CanEscape) {
					Error(&ProcessError::CanNotEscape);
					GetValue();
				}
				else {
					isEscaped = true;
				}
				return;
			}
			default:
				if (IsFractionalTooLong(c, count)) {
					Error(&ProcessError::TooLongFractional);
				}
				else {
					Error(&ProcessError::WrongCharacter);
				}
				GetValue();
				return;
			}
		}

		char* String() {
			if (type == enterType::Undefined) {
				type = enterType::String;
			}
			Start();

			if (isEscaped) {
				return nullptr;
			}

			return temp.ToString();
		}

		int Int() {
			if (type == Settings::Undefined) {
				type = Settings::Int;
			}

			Start();

			if (isEscaped) {
				return 0;
			}

			int i;
			if (temp.TryParse(i)) {
				if (HasValueLimit && (i < minValue || i > maxValue)) {
					Error(&ProcessError::WrongValue);
					return Int();
				}
				else {
					return i;
				}
			}
			else {
				Error(&ProcessError::WrongValue);
				return Int();
			}
		}
		double Double() {
			if (type == Settings::Undefined) {
				type = Settings::Double;
			}

			Start();

			if (isEscaped) {
				return 0;
			}

			double i;
			if (temp.TryParse(i)) {
				if (HasValueLimit && (i < minValue || i > maxValue)) {
					Error(&ProcessError::WrongValue);
					return Double();
				}
				else {
					return i;
				}
			}
			else {
				Error(&ProcessError::WrongValue);
				return Double();
			}
		}
	public:
		void Reset() {
			type = enterType::Undefined;
			end = endType::NewLine;
			minLenght = 1;
			maxLenght = maxMaxLenght;
			HasValueLimit = false;
			HasPointMaxNumberCountLimit = false;

			if (textForStartLine != nullptr) {
				delete[]textForStartLine;
				textForStartLine = nullptr;
			}
		}

		void operator >>(int&i) {
			int temp = Int();
			if (!isEscaped) {
				i = temp;
			}
			Finish();
		}
		void operator >>(double&i) {
			double temp = Double();
			if (!isEscaped) {
				i = temp;
			}
			Finish();
		}

		void operator >>(char*&i) {
			char* temp = String();
			if (!isEscaped) {
				i = temp;
			}
			Finish();
		}
	}get;
}
using namespace MyInput;


int main()
{
	get.end = get.Return;
	get.HasPointMaxNumberCountLimit = true;
	get.afterPointMaxNumberCount = 0;
	get.CanEscape = true;
	get.type = Get::enterType::Double;

	char* str;
	double d;
	get >> d;
	
	//printf(d);

    return 0;
}

