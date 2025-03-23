# Differentiator

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

Проект Differentiator — это инструмент для аналитического дифференцирования математических выражений, построения деревьев разбора и их оптимизации. Написан на C++ с использованием Make для сборки.

## ✨ Особенности

- **Аналитическое дифференцирование** выражений с поддержкой операций: `+`, `-`, `*`, `/`, `^` (степень).
- **Поддержка функций**: `sin`, `cos`, `ln`, `exp` и другие.
- **Упрощение выражений** (например, `x*0 → 0`, `sin(0) → 0`).
- **Парсинг выражений** из строкового формата в дерево разбора.
- **Экспорт результатов** в формат DOT для визуализации.


## 📥 Установка и сборка

### Зависимости
- Компилятор C++ с поддержкой C++17 (например, `g++` или `clang++`).
- Утилита `make`.
- Graphviz (для визуализации деревьев через DOT).

Соберите проект:

```bash
make all
```
Это создаст исполняемый файл differentiator в корне проекта.

##### Дополнительные цели Makefile
`make clean` — удалить скомпилированные объектные файлы и бинарник.

`make rebuild` — пересобрать проект с нуля.

`make test` — запустить тесты (если есть).

## 🚀 Использование
###### Программа принимает математическое выражение и возвращает его производную, упрощая результат. Пример запуска:

```bash
./differentiator "sin(x^2)"  # Производная по x
```
