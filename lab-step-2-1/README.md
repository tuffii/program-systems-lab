# lab-step-2-1

Модификация компилятора (синтаксис, вариант EX11): код, цель `make check`, отчёт `report/4/`.

После `make` выполните `make check` — сравнение `input/ex11.ass` и `input/examppl.ass` с эталонами в `expected/`.

## Структура каталога (как у `lab-step-1`)

Внутри **`lab-step-2`** должны быть только рабочие каталоги шага 2 — **без** вложенной копии `lab-step-1`. Эталон шага 1 лежит рядом: `../lab-step-1/`.

```
program-systems-lab/
├── lab-step-1/          # эталон шага 1 (не копировать внутрь step-2)
│   ├── src/
│   ├── input/
│   ├── output/
│   ├── build/
│   ├── Makefile
│   └── StartTestTask
└── lab-step-2/          # этот каталог
    ├── src/             # komppl.c, kompassr.c, absloadm.c
    ├── input/           # examppl.pli, ex11.pli, …
    ├── output/          # артефакты сборки (.pli копия, .ass, .tex, spis.mod)
    ├── build/           # собранные исполняемые файлы (имя *.exe — историческое)
    ├── report/          # отчёт (LaTeX)
    ├── Makefile
    └── StartTestTask
```

Если где-то осталась папка `lab-step-2/lab-step-1/`, её можно удалить: она дублирует репозиторий и не нужна для сборки.

## Сборка

Из каталога `lab-step-2`:

```bash
cd lab-step-2
make
```

Появятся `build/komppl.exe`, `build/kompassr.exe`, `build/absloadm.exe` (под Linux это ELF с суффиксом `.exe` из Makefile).

Требуется `gcc` с поддержкой `-m32` (и пакет `gcc-multilib` / аналог).

## Запуск только компилятора PL/I → Assembler

```bash
cd lab-step-2
make
./build/komppl.exe input/examppl.pli
# или
./build/komppl.exe input/ex11.pli
```

На выходе — файл с тем же базовым именем и расширением `.ass` рядом с логикой в `komppl.c` (см. исходник: имя берётся из аргумента).

## Полный сценарий (как в курсе): PL/I → ASM → объект → загрузчик

Интерактивный скрипт берёт **первый по сортировке** файл `*.pli` из `input/`:

```bash
cd lab-step-2
make
chmod +x StartTestTask
./StartTestTask
```

Чтобы обработать конкретный файл, переименуйте остальные `.pli` или временно оставьте в `input/` один нужный файл (скрипт использует `head -1` после сортировки).

## Через Docker (из корня репозитория, как в `lab-step-1`)

```bash
cd /path/to/program-systems-lab
docker build -t lab32 .

docker run -it --rm \
  -v "$PWD"/lab-step-2:/lab \
  -w /lab \
  lab32
```

В контейнере:

```bash
make
chmod +x StartTestTask
./StartTestTask
```

При монтировании используйте **`lab-step-2`**, а не вложенный путь.
