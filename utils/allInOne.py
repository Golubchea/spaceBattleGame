import os

# Расширения файлов, которые будем искать
EXTENSIONS = {'.hpp', '.h', '.cpp', '.c', 'CMakeLists.txt'}

# Папки, которые НЕ нужно обрабатывать
EXCLUDE_DIRS = {
    'build',
    '.git',
    '__pycache__',
    'venv',
    'externals',
    'env',
    '3rdparty'
}

def is_target_file(filename):
    # Проверяем, совпадает ли имя или расширение файла с нужными нам
    if filename.lower() == 'CMakeLists.txt':
        return True
    _, ext = os.path.splitext(filename)
    return ext in EXTENSIONS

def collect_files(root_dir):
    # Ищем все подходящие файлы рекурсивно, исключая определённые папки
    for root, dirs, files in os.walk(root_dir):
        # Убираем из поиска исключённые директории
        dirs[:] = [d for d in dirs if d not in EXCLUDE_DIRS]

        for file in files:
            if is_target_file(file.lower()):
                yield os.path.join(root, file)

def write_files_to_output(input_dir, output_file):
    with open(output_file, 'w', encoding='utf-8') as out:
        for filepath in collect_files(input_dir):
            relative_path = os.path.relpath(filepath, input_dir)
            out.write(f"// ==== ФАЙЛ: {relative_path} ====\n")
            try:
                with open(filepath, 'r', encoding='utf-8') as f:
                    content = f.read()
                    out.write(content)
            except Exception as e:
                out.write(f"// Ошибка чтения файла: {e}\n")
            out.write("\n\n")

if __name__ == "__main__":
    # Укажите здесь путь к вашей папке и имя выходного файла
    project_folder = "../."
    output_filename = "project.txt"

    write_files_to_output(project_folder, output_filename)
    print(f"Готово! Все файлы записаны в '{output_filename}'")