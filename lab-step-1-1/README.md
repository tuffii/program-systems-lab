# lab-step-1-1

Модифицированный макет компилятора PL/1 → Assembler (IBM 370): демонстрационный пример `input/examppl.pli` и персональный вариант №11 — `input/ex11.pli`.

## Структура каталога

- `src/` — исходники компиляторов и загрузчика (`.c`)
- `input/` — исходные программы на PL/1 (`*.pli`)
- `output/` — артефакты полного цикла при работе через `StartTestTask`
- `build/` — собранные `komppl.exe`, `kompassr.exe`, `absloadm.exe`
- `report/` — материалы отчёта: подкаталоги `1` … `5`, `источники`; итоговый текст — `report/5/report_komppl.tex`, эталонные листинги — `report/5/etalon/*.ass`
- `report/5/snippets/` — только для вёрстки PDF: те же программы/листинг, что `input/*.pli` и `etalon/*.ass`, но без длинных полей пробелов (иначе `listings` ломает строки по «карточным» пробелам)
- в корне — `Makefile`, `StartTestTask`

## Сборка и проверка

Образ и монтирование — из корня репозитория `program-systems-lab`, где лежит `Dockerfile`:

```bash
docker build -t lab32 -f Dockerfile .
docker run --rm -v "$PWD/lab-step-1-1":/lab:z -w /lab lab32 bash -lc 'make && make check'
```

Цель `make check` сравнивает `input/ex11.ass` и `input/examppl.ass` с `report/5/etalon/ex11.ass` и `report/5/etalon/examppl.ass`.

## Сборка PDF отчёта (опционально)

Из каталога `report/5`: два прохода **`lualatex report_komppl.tex`** (TeX Live: `fontspec`, `babel`, DejaVu). Оформление листингов как в этапах `report/1`–`report/4`, плюс `\lstset{breakautoindent=false}` и для PL/1 и ASM — `breakatwhitespace=true`. Формулы нетерминалов: `\NT{...}` (не `\text{...}` в math).
