

# WindowManager (Win32) — Полная документация API

`WindowManager` — C++ класс-обёртка над Win32 API для создания и управления
окнами, обработки системных сообщений и ввода (клавиатура / мышь).

Класс предназначен для engine-level кода:
игровые движки, графические приложения, редакторы, инструменты
(Vulkan / Direct3D / OpenGL).

---

## Общая информация

- Платформа: Windows 10+
- Язык: C++17
- Архитектура: PIMPL
- Поддержка: несколько окон
- Поток: основной (UI thread)

---

## Подключение

```cpp
#include "WindowManager.h"
````

### Зависимости

* windows.h
* Линковка: user32.lib, gdi32.lib

---

## Класс WindowManager

```cpp
class WindowManager;
```

Создаётся **только** через фабричный метод `Create`.

---

## Конфигурация окна

```cpp
struct WindowManager::Config {
    std::wstring title = L"Game Engine";
    uint32_t width  = 1280;
    uint32_t height = 720;
    int32_t posX = CW_USEDEFAULT;
    int32_t posY = CW_USEDEFAULT;
    bool resizable = true;
    bool showCursor = true;
    bool captureMouse = false;
    HICON icon = nullptr;
    HICON iconSmall = nullptr;
};
```

### Описание полей

| Поле           | Описание                    |
| -------------- | --------------------------- |
| title          | Заголовок окна              |
| width / height | Размер клиентской области   |
| posX / posY    | Начальная позиция           |
| resizable      | Разрешить изменение размера |
| showCursor     | Видимость курсора           |
| captureMouse   | Захват курсора внутри окна  |
| icon           | Большая иконка              |
| iconSmall      | Малая иконка                |

---

## Создание и уничтожение

### Create

```cpp
static std::shared_ptr<WindowManager>
Create(const Config& config = {});
```

* Регистрирует оконный класс
* Создаёт Win32 окно
* Инициализирует ввод
* В случае ошибки выбрасывает `std::runtime_error`

---

### Деструктор

```cpp
~WindowManager();
```

Автоматически освобождает все Win32-ресурсы.

---

## Обработка сообщений

### ProcessMessages

```cpp
bool ProcessMessages();
```

Должен вызываться **каждый кадр**.

Функция:

* обновляет состояние ввода
* обрабатывает Win32 сообщения
* возвращает `false`, если получен `WM_QUIT`

---

## Нативные дескрипторы

```cpp
HWND GetHWND() const;
HINSTANCE GetHInstance() const;
```

Используются для:

* Vulkan surface
* Direct3D swapchain
* низкоуровневой интеграции

---

## Размеры и состояние окна

```cpp
uint32_t GetWidth() const;
uint32_t GetHeight() const;
float GetAspectRatio() const;

bool IsMinimized() const;
bool IsMaximized() const;
bool IsActive() const;
bool IsClosed() const;
```

---

## Управление окном

```cpp
void SetTitle(const std::wstring& title);
void SetSize(uint32_t width, uint32_t height);
void SetPosition(int32_t x, int32_t y);
void CenterOnScreen();

void Minimize();
void Maximize();
void Restore();
void Show();
void Hide();

void SetResizable(bool resizable);
void SetAlwaysOnTop(bool onTop);
```

---

## Курсор и мышь

### Видимость курсора

```cpp
void SetCursorVisibility(bool visible);
```

---

### Захват мыши

```cpp
void SetMouseCapture(bool capture);
```

Ограничивает курсор границами клиентской области окна.
Используется для FPS-камер и редакторов.

---

## Ввод — клавиатура

```cpp
bool IsKeyDown(uint32_t keyCode) const;
bool IsKeyPressed(uint32_t keyCode) const;
bool IsKeyReleased(uint32_t keyCode) const;
```

| Метод         | Описание                 |
| ------------- | ------------------------ |
| IsKeyDown     | Клавиша зажата           |
| IsKeyPressed  | Нажата в текущем кадре   |
| IsKeyReleased | Отпущена в текущем кадре |

Работает корректно **только при вызове ProcessMessages каждый кадр**.

---

## Ввод — мышь

### Кнопки мыши

```cpp
bool IsMouseButtonDown(uint32_t button) const;
```

| Button | Кнопка  |
| ------ | ------- |
| 0      | Левая   |
| 1      | Правая  |
| 2      | Средняя |

---

### Позиция курсора

```cpp
void GetMousePosition(int32_t& x, int32_t& y) const;
void SetMousePosition(int32_t x, int32_t y);
```

Координаты — в клиентской области окна.

---

## Коллбэки

### Resize

```cpp
using ResizeCallback = std::function<void(uint32_t, uint32_t)>;

void SetResizeCallback(ResizeCallback callback);
```

---

### Close

```cpp
using CloseCallback = std::function<bool()>;

void SetCloseCallback(CloseCallback callback);
```

* true — разрешить закрытие
* false — отменить закрытие

---

### Общий ввод

```cpp
using InputCallback =
    std::function<void(UINT, WPARAM, LPARAM)>;

void SetInputCallback(InputCallback callback);
```

---

### Клавиатура

```cpp
using KeyCallback =
    std::function<void(uint32_t key, bool pressed)>;

void SetKeyCallback(KeyCallback callback);
```

---

### Кнопки мыши

```cpp
using MouseButtonCallback =
    std::function<void(int32_t button, bool pressed)>;

void SetMouseButtonCallback(MouseButtonCallback callback);
```

---

### Движение мыши

```cpp
using MouseMoveCallback =
    std::function<void(int32_t x, int32_t y,
                       int32_t dx, int32_t dy)>;

void SetMouseMoveCallback(MouseMoveCallback callback);
```

`dx / dy` — относительное движение курсора.

---

## Утилиты строк

```cpp
static std::wstring StringToWide(const std::string& str);
static std::string  WideToString(const std::wstring& wstr);
```

UTF-8 ↔ UTF-16 конвертация.

---

## Несколько окон

```cpp
auto win1 = WindowManager::Create({ .title = L"Main" });
auto win2 = WindowManager::Create({ .title = L"Tools" });

while (win1->ProcessMessages() &&
       win2->ProcessMessages()) {
    // todo: update / render
}
```

Каждое окно:

* независимый ввод
* собственный HWND
* собственные коллбэки

---

## Архитектурные особенности

* Нет глобальных HWND
* Нет static WindowManager*
* Используется GWLP_USERDATA
* Вся Win32-логика инкапсулирована
* Подходит для ECS и renderer-driven архитектуры

---

## Типичные ошибки

* Не вызывать ProcessMessages каждый кадр
* Использовать GetAsyncKeyState
* Писать собственный WndProc
* Хранить HWND глобально
* Обрабатывать ввод вне WindowManager

---

## Возможные расширения

* Raw Input (WM_INPUT)
* Borderless / Exclusive Fullscreen
* Input Actions / Axis
* DPI Awareness
* Gamepad (XInput / HID)

---

## Лицензия

MIT / Public Domain
Свободное использование в коммерческих и некоммерческих проектах.


