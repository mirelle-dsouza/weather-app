# Weather App

A small console program in C that asks for a city and prints its current
weather, using the [OpenWeatherMap](https://openweathermap.org/api) API for
the data plus libcurl for the HTTP request and cJSON for parsing the response.

```
Enter city name: London

--- Weather Report ---
City: London
Temperature: 4.2 °C
Condition: broken clouds
```

## Requirements

- Windows (the project is set up for Code::Blocks + MinGW)
- [Code::Blocks](https://www.codeblocks.org/) with its bundled MinGW compiler
- libcurl for MinGW-w64, extracted next to this folder as
  `curl-8.7.1_3-win64-mingw` (contains `include/`, `lib/` and `bin/`)
- A free OpenWeatherMap API key

## Setting up the API key

The key is **not** stored in the source code. It lives in `secrets.h`,
which is ignored by git:

1. Copy `secrets.h.example` to `secrets.h`.
2. Replace `PASTE_YOUR_API_KEY_HERE` with your own key.

```c
#define OPENWEATHER_API_KEY "your_key_here"
```

If `secrets.h` is missing, the program falls back to the
`OPENWEATHER_API_KEY` environment variable, e.g. in PowerShell:

```powershell
$env:OPENWEATHER_API_KEY = "your_key_here"
```

## Folder layout

The project file uses relative paths, so keep this layout:

```
Desktop/
├── Weather_App/                  <- this repository
│   ├── main.c
│   ├── cJSON.c
│   ├── secrets.h                 <- your key (not committed)
│   └── Weather_App.cbp
├── cJSON-master/                 <- not needed, cJSON is vendored in include/
└── curl-8.7.1_3-win64-mingw/
    ├── include/curl/
    ├── lib/libcurl.dll.a
    └── bin/libcurl-x64.dll
```

## Build and run in Code::Blocks

1. Open `Weather_App.cbp`.
2. Select the **Debug** target.
3. Press **F9** to build, then **Ctrl+F10** to run
   (or **F10** to build and run).
4. Copy `libcurl-x64.dll` next to the produced `bin\Debug\Weather_App.exe`
   if it is not there already.

## Build from the command line

```powershell
gcc -Wall -g -I"../curl-8.7.1_3-win64-mingw/include" -Iinclude `
    main.c cJSON.c `
    "../curl-8.7.1_3-win64-mingw/lib/libcurl.dll.a" -lws2_32 `
    -o bin/Debug/Weather_App.exe
```

## Notes / limitations

- Only single-word city names are accepted (`scanf("%s", ...)`), so
  `New York` has to be entered as `NewYork` or the input handling changed
  to `fgets`.
- The request is made over plain HTTP. OpenWeatherMap now prefers HTTPS;
  switching to `https://` requires shipping a CA bundle.
- JSON fields such as `main` / `weather` are assumed to exist; an invalid
  city name returns an error object and the program prints nothing useful.

## License

cJSON is MIT licensed (see `include/cJSON.h`). The rest of the code is
provided as-is for learning purposes.
