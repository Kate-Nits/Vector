#pragma once
#include <iostream>
#include <cstdlib> // rand()
#include <stdexcept> // std::out_of_range, std::invalid_argument

enum State { empty, busy, deleted };
template <class T>
class TVector {

	T* _data;
	State* _states;
	size_t _size;
	size_t _capacity;
	size_t _deleted;

public:
	static const size_t RESERVE_MEMORY = 15;
	static const size_t MAX_PERCENT_DELETED = 15;

	TVector() : _data(nullptr), _states(nullptr), _size(0), _capacity(0), _deleted(0) {} //конструктор по умолчанию
	TVector(size_t size) { //конструктор вектора заданного размера
		_size = size;
		_capacity = size + RESERVE_MEMORY;
		_deleted = 0;
		_data = new T[_capacity];
		_states = new State[_capacity];
		for (size_t i = 0; i < _capacity; ++i) {
			_states[i] = i < _size ? State::busy : State::empty;
		}
	}
	TVector(T* data, size_t size) { //конструктор созданный на основе переаного массива данных 
									//если передаем массив мы же можем сами размер посчитать, может не нужно у пользователя просить размер???????????????????
		_size = size;
		_capacity = size + RESERVE_MEMORY;
		_deleted = 0;
		if (size > 0 && data == nullptr) {
			throw std::invalid_argument("You sent a nullptr but with a size greater than 0");
		}
		_data = new T[_capacity];
		try {
			_states = new State[_capacity];
		}
		catch (const std::bad_alloc&) {
			delete[] _data; //тк мы же уже под _data выделили
			throw;
		}
		for (size_t i = 0; i < _capacity; ++i) {
			if (i < size) {
				_data[i] = data[i];
				_states[i] = State::busy;
			}
			else {
				_states[i] = State::empty;
			}
		}

	}
	TVector(const TVector<T>& other) { // конструктор копирования
		_size = other._size;
		_capacity = other._capacity;
		_deleted = other._deleted;
		_data = nullptr;
		_states = nullptr;
		try {
			_data = new T[_capacity];
		}
		catch (const std::bad_alloc&) { //Ловит ошибку нехватки памяти
			throw;
			std::cout << std::endl;
		}
		try {
			_states = new State[_capacity];
		}
		catch (const std::bad_alloc&) {
			delete[] _data;
			throw;
			std::cout << std::endl;
		}
		for (size_t i = 0; i < other._size; ++i) {
			_data[i] = other._data[i];
			_states[i] = other._states[i];
		}
		for (size_t i = other._size; i < other._capacity; ++i) {
			_states[i] = State::empty;
		}
	}
	~TVector() noexcept {
		delete[] _data;
		delete[] _states;
	}

	inline T& operator[](size_t index) {
		
		//check_index(index); //////////////////////

		size_t real_index = check_index(index);

		return _data[real_index];
	}
	inline const T& operator[](size_t index) const {
		size_t real_index = check_index(index);
		return _data[real_index];
	}

	inline T& at(size_t index) {
		size_t real_index = check_index(index);
		return _data[real_index];
	}
	inline const T& at(size_t index) const {
		size_t real_index = check_index(index);
		return _data[real_index];
	}

	inline const T* data() const noexcept {
		return _data;
	}

	inline const T data(size_t i) const { //оставить может надо было оставить реальность а не то что должен видеть пользователь???
		size_t real_index = check_index(i);
		return _data[real_index];
	}

	inline const State* states() const noexcept {
		return _states;
	}
	inline const State state(size_t i) const {//Cпроси что именно мы смотрим используя эту функцию (пользовательскую или программисткую часть?)
		size_t real_index = check_index(i); //Не полетит ли проверка после моего исправления 6.09???????????????
		return _states[real_index];
	}


	inline const size_t size() const noexcept {//get
		return _size;
	}
	inline void size(size_t new_size) noexcept { //set
		resize(new_size);
	}

	inline const size_t capacity() const noexcept { //get
		return _capacity;
	}
	inline void capacity(size_t new_capacity) noexcept { //set
		reserve(new_capacity);
	}



	inline T* begin() noexcept { // возвращает _data указатель на начало
		return _data;
	}
	inline T* end() noexcept {
		return _data + _size;
	}


	inline const size_t deleted() const noexcept {
		return _deleted;
	}

	inline T& front() { //Доступ к первому элементу, который не deleted и не empty
		for (size_t i = 0; i < _size; ++i)
			if (_states[i] == busy)
				return _data[i];
		throw std::out_of_range("No busy elements in vector");
	}

	inline T& back() { //Доступ к последнему элементу, который не deleted и не empty
		for (size_t i = _size; i-- > 0; )
			if (_states[i] == busy)
				return _data[i];
		throw std::out_of_range("No busy elements in vector");
	}


	inline bool is_empty() const noexcept { return _size == 0 || (_size - _deleted == 0); } //функция проверки на пустоту

	//функции вставки
	void push_front(const T& value) noexcept {
		if (is_full()) {
			reserve(_capacity + RESERVE_MEMORY);
		}

		for (size_t i = _size; i > 0; --i) { // Сдвигаем всё вправо
			_data[i] = _data[i - 1];
			_states[i] = _states[i - 1];
		}
		_data[0] = value;
		_states[0] = busy;
		++_size;
	}
	void push_back(const T& value) noexcept {
		if (is_full()) {
			reserve(_capacity + RESERVE_MEMORY);
		}
		_data[_size] = value;
		_states[_size] = busy;
		++_size;
	}
	void insert(size_t index, const T& value) {
		if (index > _size) {
			throw std::out_of_range("Insert index out of range");
		}
		if (is_full()) {
			reserve(_capacity + RESERVE_MEMORY);
		}
		for (size_t i = _size; i > index; --i) { // Сдвигаем вправо
			_data[i] = _data[i - 1];
			_states[i] = _states[i - 1];
		}
		_data[index] = value;
		_states[index] = busy;
		++_size;
	}

	//функции удаления
	void pop_front() {
		if (_data != nullptr && _states != nullptr) {
			for (size_t i = 0; i < _size; ++i) {
				if (_states[i] == State::busy) {
					_states[i] = State::deleted;
					++_deleted;
					if (_size > 0 && ((_deleted * 100 / _size) > MAX_PERCENT_DELETED)) {
						shrink_to_fit();
					}
					return;
				}
			}
		}
		throw std::underflow_error("Vector is empty");
	}
	void pop_back() {
		if (_data != nullptr && _states != nullptr) {
			for (size_t i = _size - 1; ; --i) {
				if (_states[i] == State::deleted) {
					_states[i] = State::empty;
					--_size;
					--_deleted;
				}
				if (_states[i] == State::busy) {
					_states[i] = State::empty;
					--_size;
					return;
				}
				if (i <= 0) break; // иначе уйдём в переполнение
			}
		}
		throw std::underflow_error("Vector is empty");
	}
	void erase(size_t index) {
		size_t real_index = check_index(index);
		_states[real_index] = State::deleted;
		++_deleted;
		if (_size > 0 && ((_deleted * 100 / _size) > MAX_PERCENT_DELETED)) {
			shrink_to_fit();
		}
	}

	//замена значения
	/*void emplace(size_t index, T&& value) { //со сдвигом
		check_index(index);
		if (is_full()) {
			reserve(_capacity + RESERVE_MEMORY);
		}
		for (size_t i = _size; i > index; --i) {
			_data[i] = _data[i - 1];
			_states[i] = _states[i - 1];
		}
		_data[index] = value;
		_states[index] = State::busy;
		++_size;
	}*/
	void emplace(size_t index, T&& value) {
		size_t real_index = check_index(index);
		_data[real_index] = value;
		_states[real_index] = State::busy;
	}

	TVector<T>& assign(const TVector<T>& other) {
		if (this != &other) {
			delete[] _data;
			delete[] _states;

			_size = other._size;
			_capacity = other._capacity;
			_deleted = other._deleted;

			_data = new T[_capacity];
			_states = new State[_capacity];
			for (size_t i = 0; i < _size; ++i) {
				_data[i] = other._data[i];
				_states[i] = other._states[i];
			}
			for (size_t i = _size; i < _capacity; ++i)
				_states[i] = State::empty;
		}
		return *this;
	}

	TVector<T>& operator=(const TVector<T>& other) noexcept {
		if (this != &other) { //проверка на самоприсваивание
			delete[] _data;
			delete[] _states;

			_size = other._size;
			_capacity = other._capacity;
			_deleted = other._deleted;

			_data = new T[_capacity];
			_states = new State[_capacity];

			for (size_t i = 0; i < _size; ++i) {
				_data[i] = other._data[i];
				_states[i] = other._states[i];
			}
			for (size_t i = _size; i < _capacity; ++i)
				_states[i] = State::empty;
		}
		return *this;
	}

	void clear() noexcept {
		for (size_t i = 0; i < _size; ++i) {
			_states[i] = State::empty;
		}
		_size = 0;
		_deleted = 0;
	}

	bool operator == (const TVector<T>& other) const noexcept {
		if (_size != other._size) {
			return false;
		}
		for (size_t i = 0; i < _size; ++i) {
			if (_states[i] == busy && _data[i] != other._data[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator != (const TVector<T>& other) const noexcept {
		if (*this == other) {
			return false;
		}
		if (_size != other._size) {
			return true;
		}
		for (size_t i = 0; i < _size; ++i) {
			if (_states[i] != other._states[i]) {
				return true;
			}
			if (_states[i] == busy && _data[i] != other._data[i]) {
				return true;
			}
		}
		return false;
	}


	void reserve(size_t new_capacity) { //увеличивает _capacity
		if (new_capacity <= _capacity) return;

		T* new_data = new T[new_capacity];
		State* new_states = new State[new_capacity];

		for (size_t i = 0; i < _size; ++i) { // Копируем существующие элементы
			new_data[i] = _data[i];
			new_states[i] = _states[i];
		}

		for (size_t i = _size; i < new_capacity; ++i) { // Помечаем оставшиеся ячейки как пустые
			new_states[i] = State::empty;
		}

		delete[] _data;
		delete[] _states;
		_data = new_data;
		_states = new_states;
		_capacity = new_capacity;
	}

	void resize(size_t new_size) {
		if (new_size < _size) {
			for (size_t i = new_size; i < _size; ++i) {
				if (_states[i] == State::deleted)
					--_deleted;
				_states[i] = State::empty;
				_data[i].~T();
			}
			_size = new_size;
			return;
		}

		if (new_size > _capacity) {
			reserve(new_size + RESERVE_MEMORY);
		}

		for (size_t i = _size; i < new_size; ++i) {
			new (_data + i) T(); //размещает результат в уже выделенной памяти по адресу _data + i
			_states[i] = busy;
		}
		_size = new_size;
	}

	void resize(size_t new_size, const T& value) {
		if (new_size == _size) {
			return;
		}

		if (new_size < _size) {
			for (size_t i = new_size; i < _size; ++i) {
				if (_states[i] == deleted)
					--_deleted;
				_states[i] = empty;
				_data[i].~T();
			}
			_size = new_size;
			return;
		}

		if (new_size > _capacity) {
			reserve(new_size + RESERVE_MEMORY);
		}

		for (size_t i = _size; i < new_size; ++i) {
			new (_data + i) T(value); //размещает результат в уже выделенной памяти по адресу _data + i
			_states[i] = busy;
		}
		_size = new_size;
	}

	void shrink_to_fit() { //уменьшение размера, удаляя неиспользуемую память
		size_t busy_count = 0;

		for (size_t i = 0; i < _size; ++i) { // Считаем busy элементы
			if (_states[i] == busy)
				++busy_count;
		}

		if (busy_count == _size && _capacity == _size)
			return;

		size_t new_capacity = busy_count + RESERVE_MEMORY;
		T* new_data = new T[new_capacity];
		State* new_states = new State[new_capacity];

		size_t new_index = 0;
		for (size_t i = 0; i < _size; ++i) {
			if (_states[i] == busy) {
				new_data[new_index] = _data[i];
				new_states[new_index] = busy;
				++new_index;
			}
		}

		for (size_t i = new_index; i < new_capacity; ++i) {
			new_states[i] = empty;
		}

		delete[] _data;
		delete[] _states;

		_data = new_data;
		_states = new_states;
		_capacity = new_capacity;
		_size = busy_count;
		_deleted = 0;
	}

	void print() {
		std::cout << "[ ";
		for (size_t i = 0; i < _size; ++i) {
			if (_states[i] == State::busy) {
				std::cout << _data[i] << "  ";
			}
		}
		std::cout << "]";
	}
	template <class U>
	friend std::ostream& operator<<(std::ostream& out, const TVector<U>& vec);


	template <typename U>
	friend void swap(U& a, U& b);
	template <typename U>
	friend void shuffle(TVector<U>& vec);
	template <typename U>
	friend void quick_sort(TVector<U>& vec);
	template <typename U>
	friend size_t find_first(const TVector<U>& vec, const U& value);
	template <typename U>
	friend size_t find_last(const TVector<U>& vec, const U& value);
	template <typename U>
	friend size_t* find_all(const TVector<U>& vec, const U& value);
	template <typename U>
	friend void quick_sort_realisation(TVector<U>& vec, int left, int right);

private:

	/*void check_index(size_t index) const { //версия до 6.09 (после правок Марины Андреевны
		if (index >= _size) {
			throw std::out_of_range("Index out of bounds: index >= size");
		}
		if (_states[index] != State::busy) {
			throw std::out_of_range("Accessing element that is not active (deleted or empty)");
		}
	}*/
	/*void check_index(size_t index) const {
		if (index >= _size) {
			throw std::out_of_range("Index out of bounds: index >= size");
		}
		size_t count = 0;
		for (size_t i = 0; i < _size; i++) {
			if (_states[i] == State::busy) {
				if (count == index) {
					return;
				}
				count++;
			}
		}
		throw std::out_of_range("There no element with this index");
	}*/
	size_t check_index(size_t index) const {
		if (index >= _size) {
			throw std::out_of_range("Index out of bounds: index >= size");
		}
		size_t count = 0;
		for (size_t i = 0; i < _size; i++) {
			if (_states[i] == State::busy) {
				if (count == index) {
					return i;
				}
				count++;
			}
		}
		throw std::out_of_range("There no element with this index");
	}
	inline bool is_full() const noexcept { //функция проверки на заполненость
		return _size >= _capacity;
	}
	template <class>
	friend class TVectorTester;

};

template <typename T>
void swap(T& a, T& b) {
	T temp = a;
	a = b;
	b = temp;
}

template <typename T>
void shuffle(TVector<T>& vec) {
	if (vec._size <= 1) {
		return;
	}
	for (size_t i = vec._size - 1; i > 0; --i) {
		if (vec._states[i] != busy) {
			continue;
		}
		size_t j = std::rand() % (i + 1);
		while (vec._states[j] != busy) {
			j = std::rand() % (i + 1);
		}
		swap(vec._data[i], vec._data[j]);
	}
}

/*
template <typename T>
int partition(TVector<T>& vec, int left, int right) {
	T p = vec._data[right];
	int i = left - 1;

	for (int j = left; j < right; ++j) {
		if (vec._states[j] == busy && vec._data[j] <= p) {
			++i;
			swap(vec._data[i], vec._data[j]);
		}
	}
	swap(vec._data[i + 1], vec._data[right]);
	return i + 1;
}

template <class T>
void quick_sort_realisation(TVector<T>& vec, int left, int right) {
	if (left < right) {
		int p = partition(vec, left, right);
		quick_sort_realisation(vec, left, p - 1);
		quick_sort_realisation(vec, p + 1, right);
	}
}
*/


template <class T>
void quick_sort_realisation(TVector<T>& vec, int left, int right) {
	if (left > right) {
		return;
	}
	T p = vec[(left + right) / 2];
	int i = left;
	int j = right;
	while (i <= j) {
		while (vec[i] < p) i++;
		while (vec[j] > p) j--;
		if (i <= j) {
			swap(vec._data[i], vec._data[j]);
			swap(vec._states[i], vec._states[j]);
			i++;
			j--;
		}
	}
	quick_sort_realisation(vec, left, j);
	quick_sort_realisation(vec, i, right);
}
template <class T>
void quick_sort(TVector<T>& vec) {
	if (vec.size() > 1)
		quick_sort_realisation(vec, 0, vec.size() - 1);
}

template <class T>
size_t find_first(const TVector<T>& vec, const T& value) {
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec.state(i) == State::busy && vec.data(i) == value) {
			return i;
		}
	throw std::logic_error("Element not found");
}
template <class T>
size_t find_last(const TVector<T>& vec, const T& value) {
	for (size_t i = vec._size; i-- > 0;)
		if (vec.state(i) == State::busy && vec.data(i) == value) {
			return i;
		}
	throw std::logic_error("Element not found");
}
template <class T>
size_t* find_all(const TVector<T>& vec, const T& value) {
	size_t result_size = 0;

	for (size_t i = 0; i < vec.size(); ++i) { // Считаем количество подходящих элементов
		if (vec.state(i) == State::busy && vec.data(i) == value) {
			++result_size;
		}
	}

	if (result_size == 0) {
		return nullptr;
	}
	size_t* indices = new size_t[result_size];
	size_t j = 0;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec.state(i) == State::busy && vec.data(i) == value) {
			indices[j++] = i;
		}
	}
	return indices;
}

template <class T>
std::ostream& operator<<(std::ostream& out, const TVector<T>& vec) {
	out << "[ ";
	for (size_t i = 0; i < vec._size; ++i) {
		if (vec._states[i] == State::busy) {
			out << vec._data[i] << " ";
		}
	}
	out << "]";
	return out;
}
