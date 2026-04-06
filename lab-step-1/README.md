step1 — адаптация исходного кода примера под современную кодировку и отображение в терминале.

Структура каталога `lab-step-1`:

- `src/` — исходники компиляторов и загрузчика (`.c`)
- `input/` — сюда кладётся PL/1-программа (например `examppl.pli`)
- `output/` — сюда попадают артефакты цепочки (копия `.pli`, `.ass`, `.tex`, генерируемый `spis.mod`)
- `build/` — собранные `*.exe` (историческое расширение, под Linux это ELF)
- в корне — `Makefile`, `StartTestTask`

Способ запуска:

```bash
docker build -t lab32 .

docker run -it --rm \
  -v "$PWD"/lab-step-1:/lab \
  -w /lab \
  lab32
```

Внутри контейнера:

```bash
make
chmod +x StartTestTask
./StartTestTask
```

Скрипт берёт первый по имени файл `*.pli` из `input/`, копирует его в `output/` и запускает полный цикл с путями в `output/`.
