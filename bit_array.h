#ifndef BIT_ARRAY_H
#define BIT_ARRAY_H

#include <string>
#include <stdint.h>
typedef uint32_t byte4;
typedef unsigned char byte;

struct IndexErrorException
{
    std::string error;
    IndexErrorException(std::string str)
    {
        this->error = str;
    }
};

struct MemoryErrorException
{
    std::string error;
    MemoryErrorException(std::string str)
    {
        this->error = str;
    }
};

class BitArray
{
private:
    size_t num_bits;
    size_t real_size;
    byte4* array;
    byte* ptr;
public:
    BitArray();
    ~BitArray();

    //Конструирует массив, хранящий заданное количество бит.
    //Первые sizeof(long) бит можно инициализровать с помощью параметра value.
    explicit BitArray(size_t num_bits, unsigned long value = 0);
    BitArray(const BitArray& b);
 
    //Обменивает значения двух битовых массивов.
    void swap(BitArray& b);

    BitArray& operator=(const BitArray& b);

    //Изменяет размер массива. В случае расширения, новые элементы 
    //инициализируются значением value.
    void resize(size_t num_bits, bool value = false);

    //Очищает массив.
    void clear();
    //Добавляет новый бит в конец массива. В случае необходимости 
    //происходит перераспределение памяти.
    void push_back(bool bit);

    //Битовые операции над массивами.
    //Работают только на массивах одинакового размера.
    //Обоснование реакции на параметр неверного размера входит в задачу.
    BitArray& operator&=(const BitArray& b);
    BitArray& operator|=(const BitArray& b);
    BitArray& operator^=(const BitArray& b);

    ////Битовый сдвиг с заполнением нулями.
    BitArray& operator<<=(size_t n);
    BitArray& operator>>=(size_t n);
    BitArray operator<<(size_t n) const;
    BitArray operator>>(size_t n) const;

    //Устанавливает бит с индексом n в значение val.
    BitArray& set(size_t n, bool val = true);
    //Заполняет массив истиной.
    BitArray& set();

    //Устанавливает бит с индексом n в значение false.
    BitArray& reset(size_t index);
    //Заполняет массив ложью.
    BitArray& reset();

    //true, если массив содержит истинный бит.
    bool any() const;
    //true, если все биты массива ложны.
    bool none() const;
    ////Битовая инверсия
    BitArray operator~() const;
    //Подсчитывает количество единичных бит.
    size_t count() const;

    //Возвращает значение бита по индексу i.
    bool operator[](size_t index) const;

    size_t size_in_bits() const;

    size_t size_in_bytes() const;

    bool empty() const;

    //Возвращает строковое представление массива.
    std::string to_string() const;
};

#endif