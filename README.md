## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch--islamic--gs--modular--imgui-orange)

[![Fund The Experiments](https://img.shields.io/badge/Fund-The_Experiments-FF5722?style=for-the-badge&logo=buy-me-a-coffee)](https://sociabuzz.com/abdkdhni)

---

## 📺 Demo Video

Lihat hasilnya di YouTube: [Watch Demo](https://youtu.be/_VGDcTeWVa0)

---

## 📺 Preview

Project ini menampilkan **pola geometri Islam dasar** dengan lima lingkaran yang saling berhubungan, crosslines, parallelogram lines, rectangle lines, dan octagram lines:

- **5 CircleShape (A, B, C, D, E)** - Lingkaran dengan posisi kardinal
- **Cartesian Axes** - Sistem koordinat X-Y dengan label sudut (radians & degrees)
- **4 CrossLine (F, G, H, I)** - Garis diagonal dari center ke sudut dengan dot di intersection
- **4 ParallelogramLine (N, O, P, Q)** - Garis penghubung antar circle center dengan dot di intersection
- **4 RectangleLine (F→G, G→I, I→H, H→F)** - Garis pembentuk rectangle dengan 2 dot di intersection
- **8 OctagramLine (0, 1, 2, 3, 4, 5, 6, 7)** - Garis pembentuk pola octagram (8-point star) dengan 2 garis per line

Setiap shape memiliki **animasi drawing** yang halus, label yang dinamis, dot di posisi intersection, dan line width yang dapat diadjust.

---

## ✨ Fitur & Teknik

### Geometric Pattern System
- **Template System** - Modular `SacredGeometryTemplate` base class untuk extensibility pattern yang mudah
- **Template Registry** - Singleton pattern untuk managing multiple geometric patterns
- **BasicZelligeTemplate** - Moroccan Islamic geometric pattern dengan 8-fold symmetry (26 shapes)
- **Five Circle Pattern** - 5 lingkaran dengan radius dinamis yang saling berhubungan
- **CrossLine System** - 4 garis diagonal dari center ke sudut dengan dot di intersection point
- **Parallelogram Lines** - 4 garis penghubung antar circle center membentuk diamond/rhombus
- **Rectangle Lines** - 4 garis pembentuk rectangle (F→G, G→I, I→H, H→F) dengan 2 dot di intersection
- **Octagram Lines** - 8 garis pembentuk pola octagram (8-point star) dengan 2 segment per line (main + extension)

### Runtime Updates & Scalability
- **Runtime Radius Updates** - Semua shapes update posisinya secara realtime saat radius diubah via slider
- **Proportional Scaling System** - Posisi di-calculate ulang secara proporsional menggunakan `scaleFactor = newRadius / originalRadius`
- **Polar Thinking** - Perhitungan posisi menggunakan trigonometri (cos, sin, atan2) untuk scalability
- **Angle/Distance Approach** - CircleShape menggunakan angle + distance bukan Cartesian coordinates untuk better scaling
- **Scalable Dot Positions System** - Semua dot positions diambil langsung dari shape objects, TANPA hardcoded values
- **Scalable Intersection Points** - Semua intersection dihitung dengan rumus geometric yang scalable terhadap radiusCircle
- **Cached Dot Auto-Rebuild** - Dot cache otomatis di-rebuild saat radius berubah untuk akurat mouse hover/drag detection

### GUI System (ImGui Integration)
- **ImGui Integration** - Complete ImGui dengan OpenGL3 dan Win32 backends
- **AbstractGuiComponent** - Base class untuk reusable GUI components
- **MenuBar** - Top menu bar dengan File, Edit, dan View menus
  - **File Menu**: Save Workspace (CTRL+S), Save As (CTRL+SHIFT+S), Open (CTRL+O), Exit (ALT+F4)
  - **Edit Menu**: Undo (CTRL+Z), Redo (CTRL+SHIFT+Z), Delete All Custom Lines, Delete All Polygons, Delete Lines & Polygons (CTRL+DEL), Clean Canvas (CTRL+SHIFT+DEL)
  - **View Menu**: Sacred Geometry, Playground (show/focus windows independently)
- **SacredGeometry Panel** - Template controls panel dengan:
  - Template Name Display (Basic Zellige)
  - Draw Mode: Parallel, Sequential, Hide radio buttons
  - Radius Slider (50-240) - real-time scaling
  - Line Width Slider (0-4px)
  - Labels, Dots, Cartesian checkboxes
  - **Custom Line Color Picker** - Circular color wheel untuk custom line color (default: blue)
  - **Polygon Color Picker** - Circular color wheel untuk polygon color (default: blue)
  - Clean Canvas Button
  - Template-Specific Settings (collapsible)
- **Playground Panel** - Playback panel dengan:
  - Opened File Display
  - Parallel/Sequential Per Group radio buttons
  - Auto Clean Canvas checkbox
  - Play Arrow Button
  - Template-specific playback UI (BasicZellige has playback controls)
- **Independent Window Management** - SacredGeometry dan Playground windows bisa di-close (X button) dan di-show secara independen melalui View menu
- **SuccessPopup** - Modal popup untuk save/load success confirmations
- **ErrorPopup** - Modal popup untuk error handling
- **Realtime Controls** - Semua settings apply secara realtime tanpa restart
- **G Key Toggle** - Tekan G untuk toggle ImGui: MenuBar only → All windows → Hide all

### Animation System
- **Sequential Drawing Mode** - Animasi drawing berurutan dari satu shape ke shape berikutnya
- **Parallel Drawing Mode** - Animasi drawing parallel untuk semua shapes sekaligus
- **Animated Drawing** - Semua shape digambar dengan animasi smooth (100 segments, configurable speed)
- **Bidirectional Animation** - Animasi muncul (show) dan hilang (hide) dengan smoothing
- **Two-Phase Animation** - OctagramLine memiliki 2 mode: sequential (Phase 1 → Phase 2) dan paralel (barengan)
- **Polygon Animation System** - Base class `AbstractAnimation` untuk reusable animations:
  - **FadeInAnimation** - Alpha blending fade-in (0 → targetAlpha)
  - **WobbleAnimation** - Oscillation effect dengan amplitude dan frequency
  - **FillAnimation** - Directional fill (bottom to top) dengan wave effect
- **Configurable Speed** - Animation speed dapat di-adjust via Playground panel

### Custom Lines & Polygons
- **Mouse Interaction** - Mouse drag untuk menggambar line secara interaktif antar dots
- **CustomLine System** - Garis custom yang bisa dibuat user dengan mouse drag (start dot → end dot)
- **Bezier Curve Support** - Garis bisa dibuat melengkung dengan scroll mouse (curve parameter)
- **Multi-Select System** - CTRL+Klik untuk toggle selection multiple garis
- **CustomLine Labels** - Saat selected, muncul label "CustomLine0", "CustomLine1", dst (bukan ganti warna)
- **Curve Label** - Display nilai curve saat garis di-select
- **PolygonShape System** - Class untuk polygon fill-only tanpa outline (hanya warna)
- **Create Polygon (CTRL+G)** - Buat polygon dari selected customLines (otomatis deteksi closed loop)
- **Polygon Color Preset** - 9 warna preset untuk polygon (merah, hijau, biru, kuning, magenta, cyan, orange, ungu, abu-abu)
- **Color Picker Integration** - Ubah warna custom line dan polygon secara real-time via color picker di SacredGeometry panel
- **100-Step Undo/Redo System** - Complete undo/redo dengan full state tracking:
  - CREATE_LINE / CREATE_POLYGON
  - CHANGE_COLOR_LINE / CHANGE_COLOR_POLYGON (multi-select support)
  - DELETE_LINE / DELETE_POLYGON
  - CHANGE_CURVE (multi-select support)
- **Delete All Custom Lines** - Hapus semua customLines saja (Edit menu)
- **Delete All Polygons** - Hapus semua polygons saja (Edit menu)

### Workspace Save/Load
- **Centralized Save/Load** - Satu method `saveWorkspace()`, `saveWorkspaceAs()`, dan `loadWorkspace()` untuk semua state
- **.nay Binary Format** - Workspace format dengan magic number "NA01", version 2
- **Complete State Persistence** - Save template name, radius, custom lines, polygons, semua settings (labels, dots, line width, cartesian visibility)
- **Direct File Save** - Save langsung ke filepath target tanpa intermediate "workspace.nay" file
- **File Dialog Integration** - Native Windows file dialog untuk Save As/Open operations dengan .nay filter validation
- **Two Load Modes**:
  - **Parallel (CTRL+O)**: Template → CustomLines → Polygons animate simultaneously per group
  - **Sequential (CTRL+SHIFT+O)**: Groups animate satu per satu dengan delay
- **Auto Clean Canvas** - Otomatis bersihkan canvas sebelum load jika checkbox dicentang
- **Delay Load System** - Smooth transition dengan delay sebelum animation starts
- **Animation State Preservation** - State animasi di-save dan di-restore dengan benar
- **Playground Auto-Focus** - Saat file berhasil dibuka, Playground window otomatis muncul dan focus
- **Error Handling** - Comprehensive error handling untuk no file selected, invalid format, canvas not empty, no mode selected
- **Color Picker Sync** - Color picker otomatis sync dengan warna customLines/polygons yang diload

### Performance & Rendering
- **Performance Optimization - Cached Dots** - Sistem lazy cache untuk getAllDots() dengan dirty flag, hanya rebuild saat visibility berubah (reduce dari 180 vector copies/detik menjadi 0)
- **Anti-Aliasing & Smoothing** - Garis yang smooth untuk visual yang lebih baik
- **60 FPS Performance** - Solid 60 FPS pada resolusi bervariasi dengan CPU-based rendering
- **Trails Effect** - Semi-transparent overlay untuk efek jejak visual yang menarik

### Display & Controls
- **Cartesian Coordinate System** - Sumbu X-Y dengan animasi scaling (0 to 2.5x radius) dan label sudut (radians & degrees)
- **Dynamic Line Width** - Ketebalan garis yang dapat diadjust secara realtime (0 - 4px)
- **Static Font System** - Semua label menggunakan font normal (Calibri 15px) untuk konsistensi visual
- **Angle Labels** - Label sudut pada CartesianAxes menampilkan format "radians (degrees)" di setiap ujung sumbu
- **Label Toggle** - Show/hide label untuk semua shapes
- **Dot Toggle** - Show/hide dot di intersection points
- **Individual Controls** - Toggle CartesianAxes terpisah dari shapes lain
- **Comprehensive Keyboard Shortcuts** - 30+ keyboard shortcuts untuk power users

### Architecture & Code Quality
- **AbstractShape Base Class** - Arsitektur OOP dengan inheritance untuk code reusability
- **Template Method Pattern** - `SacredGeometryTemplate` untuk extensibility geometric patterns
- **Strategy Pattern** - UpdateState enum (NORMAL, SEQUENTIAL_DRAWING, DELAYED_LOAD, STAGGERED_LOAD)
- **Singleton Pattern** - TemplateRegistry untuk managing templates
- **Component Pattern** - AbstractGuiComponent untuk reusable GUI components
- **Factory Pattern** - Template dan polygon creation through registry
- **State Pattern** - Drawing states (IDLE, DRAGGING) dan load states (LoadStage)
- **Smart Pointer Management** - Menggunakan `std::unique_ptr` dan `std::shared_ptr` untuk resource management
- **Modular Design** - Terpisah dalam kategori: `shape/`, `anim/`, `operation/`, `template/`, `operation/gui/`
- **Memory-safe Implementation** - Modern C++17 features dengan RAII, smart pointers, move semantics
- **No Memory Leaks** - Comprehensive memory management dengan automatic cleanup

---

## 🎮 Controls

### GUI Controls (ImGui Panel)

**Sacred Geometry Panel Settings:**
| Control | Action |
| --- | --- |
| **Template Name Display** | Menampilkan nama template yang sedang aktif (Basic Zellige) |
| **Parallel Radio Button** | Show semua shapes secara parallel (barengan) |
| **Sequential Radio Button** | Start sequential drawing animation (shapes muncul berurutan) |
| **Hide Radio Button** | Hide semua shapes |
| **Radius Slider** | Adjust template radius (50 - 240) secara realtime - semua shapes update posisinya secara proporsional |
| **Line Width Slider** | Adjust ketebalan garis (0 - 4px) untuk semua shapes |
| **Labels Checkbox** | Toggle visibility untuk semua label shapes |
| **Dots Checkbox** | Toggle visibility untuk semua dots di intersection points |
| **Cartesian Checkbox** | Toggle visibility untuk CartesianAxes secara terpisah |
| **Line Color Picker** | Pilih warna untuk custom lines (circular color wheel with alpha bar) |
| **Polygon Color Picker** | Pilih warna untuk selected polygon (circular color wheel with alpha bar) |
| **Clean Canvas Button** | Clear semua polygons, custom lines, dan hide template shapes |
| **Template-Specific** | Collapsible section untuk template-specific settings (BasicZellige has additional controls) |

**Playground Panel:**
| Control | Action |
| --- | --- |
| **Opened File Display** | Menampilkan nama file .nay yang terakhir dibuka |
| **Parallel Per Group Radio Button** | Set draw mode ke parallel (semua groups animate barengan) |
| **Sequential Per Group Radio Button** | Set draw mode ke sequential (groups animate satu per satu) |
| **Auto Clean Canvas Checkbox** | Otomatis bersihkan canvas sebelum load workspace |
| **Play Arrow Button** | Load dan animate workspace sesuai mode yang dipilih (dengan delay untuk smooth transition) |

**MenuBar (File Menu):**
| Menu Item | Action | Keyboard Shortcut |
| --- | --- | --- |
| **Save Workspace** | Simpan semua state ke lastSavedPath | **CTRL+S** |
| **Save As...** | Simpan workspace ke lokasi custom (file dialog) | **CTRL+SHIFT+S** |
| **Open...** | Buka file dialog untuk load workspace .nay | **CTRL+O** |
| **Exit** | Keluar dari aplikasi | **ALT+F4** |

**MenuBar (Edit Menu):**
| Menu Item | Action | Keyboard Shortcut |
| --- | --- | --- |
| **Undo** | Undo last action | **CTRL+Z** |
| **Redo** | Redo last undone action | **CTRL+SHIFT+Z** |
| **Delete All Custom Lines** | Hapus semua custom lines saja | - |
| **Delete All Polygons** | Hapus semua polygons saja | - |
| **Delete Lines & Polygons** | Hapus semua custom lines dan polygons | **CTRL+DEL** |
| **Clean Canvas** | Clear semua dan hide template shapes | **CTRL+SHIFT+DEL** |

**MenuBar (View Menu):**
| Menu Item | Action |
| --- | --- |
| **Sacred Geometry** | Show SacredGeometry window (hidden) atau focus ke window (already visible) |
| **Playground** | Show Playground window (hidden) atau focus ke window (already visible) |

**Window Management:**
- **Close Button (X)** - Setiap window (SacredGeometry, Playground) punya tombol close di pojok kanan atas
- **Independent Visibility** - Window bisa di-close dan di-show secara independen
- **Auto-Focus** - Saat file berhasil dibuka, Playground window otomatis muncul dan focus

**Success Popup:**
- Muncul setelah save berhasil
- Menampilkan pesan konfirmasi
- Auto-close setelah klik OK atau anywhere

**Error Popup:**
- Muncul saat ada error (invalid file format, no file selected)
- Menampilkan pesan error dengan tombol OK
- Muncul dengan MenuBar (tanpa show windows lain)

---

### Keyboard Controls

**ImGui Toggle:**
| Input | Action |
| --- | --- |
| **G** atau **g** | Toggle ImGui visibility: MenuBar only → All windows → Hide all |

**Main Canvas Controls:**
| Input | Action |
| --- | --- |
| **SHIFT + 1** atau **SHIFT + !** | Sequential drawing - shapes muncul berurutan |
| **SHIFT + )** | Show semua shapes |
| **DEL** | Hide semua shapes (termasuk CartesianAxes) |
| **BACKSPACE** | Toggle CartesianAxes saja ATAU hapus selected line/polygon |
| **\`** atau **~** | Toggle label visibility (semua label) |
| **.** atau **>** | Toggle dot visibility (semua dot di intersection points) |
| **+** atau **=** | Increase line width (+0.5px, max 4px) |
| **-** atau **_** | Decrease line width (-0.5px, min 0px) |
| **END** | Keluar dari aplikasi |

**CustomLine Controls:**
| Input | Action |
| --- | --- |
| **Mouse Drag** | Buat garis custom dari dot ke dot (hanya bisa mulai dan akhir di dot) |
| **Mouse Click** | Select garis custom (muncul label "CustomLine0", dst) |
| **CTRL + Click** | Toggle selection multiple garis (multi-select) |
| **SHIFT + B** | Select semua customLines sekaligus |
| **Mouse Scroll** | Adjust curvature garis yang selected (scroll up/down, 0 = lurus) |
| **1 - 9** | Assign color ke selected polygon (1=Merah, ..., 9=Abu-abu) |

**Undo/Redo Controls:**
| Input | Action |
| --- | --- |
| **CTRL + Z** | Undo last action (100-step history) |
| **CTRL + SHIFT + Z** atau **CTRL + Y** | Redo last undone action |

**Polygon Controls:**
| Input | Action |
| --- | --- |
| **Mouse Click** | Select polygon (muncul label "Polygon0", dst di tengah) |
| **1 - 9** | Ganti warna selected polygon secara realtime |
| **CTRL + G** | Buat polygon dari selected customLines (min 3 garis) |
| **CTRL + DEL** | Hapus semua polygons dan custom lines |
| **BACKSPACE** | Hapus selected polygon ATAU toggle CartesianAxes |

**File Operations:**
| Input | Action |
| --- | --- |
| **CTRL + S** | Save workspace ke lastSavedPath |
| **CTRL + SHIFT + S** | Save workspace ke custom location |
| **CTRL + O** | Buka file dialog untuk load workspace .nay |
| **CTRL + SHIFT + O** | Load workspace dengan sequential animation |

---

## 🛠️ Tech Stack

- **[OpenFrameworks 0.12.1](https://openframeworks.cc/)** - Creative coding framework
- **[ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI library
- **[ImGui for OpenFrameworks](https://github.com/jvcleave/ofxImGui)** - ImGui integration
- **C++17** - Modern C++ features (RAII, smart pointers, move semantics)
- **Visual Studio 2022 Community** - IDE dan compiler
- **OpenGL 4.6** - Rendering backend
- **glm (OpenGL Mathematics)** - Vector math library (included in OpenFrameworks)

---

## 📦 Installation

### Prerequisites

1. Install **[OpenFrameworks](https://openframeworks.cc/)** untuk Windows
2. Install **Visual Studio 2022** dengan workload "Desktop development with C++"

### Setup Project

```bash
# Clone repository ini
git clone https://github.com/creativecoding-tech/BasicIslamicGeometry.git

# Checkout branch sketch-islamic-gs-modular-imgui
git checkout sketch-islamic-gs-modular-imgui

# Jalankan OpenFrameworks Project Generator
# Buka: openFrameworks/apps/projectGenerator/projectGenerator.exe

# Import project yang sudah diclone
# 1. Click "Import" pada Project Generator
# 2. Navigate ke folder BasicIslamicGeometry yang sudah diclone
# 3. Select folder dan klik "Select Current Folder"
# 4. Click "Update" atau "Generate"

# Buka Visual Studio
# Double-click: BasicIslamicGeometry.sln

# Build & Run
# Press F5 atau klik "Local Windows Debugger"
```

---

## 🧬 Mathematics Behind

### Circle Drawing Animation

Setiap lingkaran digambar dengan animasi **segment-based drawing**:

```cpp
// Map dari 0-100 segments ke 0-TWO_PI
float drawAngle = ofMap(i, 0, totalSegments, 0, TWO_PI);

// Untuk segment terakhir, kembali ke angle 0 (tutup loop)
if (i == totalSegments && progress >= totalSegments) {
    drawAngle = 0;  // Kembali ke titik awal
}

// Hitung posisi X, Y untuk setiap segment
float px = cos(drawAngle) * radius;
float py = sin(drawAngle) * radius;
```

**Parameter Animation:**
- **totalSegments**: 100 segments untuk full circle
- **speed**: Variable speed per shape (drawing speed)
- **lineWidth**: 0px (tipis) sampai 4px (tebal)

### Cartesian Axes Scaling

Sumbu cartesian memiliki animasi **scaling** untuk reference posisi:

```cpp
// Scale dari 0 ke 2.5x radius
float currentLength = radius * progress;
// Draw sumbu X dari -currentLength ke currentLength
ofDrawLine(-currentLength, 0, currentLength, 0);
// Draw sumbu Y dari -currentLength ke currentLength
ofDrawLine(0, -currentLength, 0, currentLength);
```

**Parameter Scaling:**
- **maxScale**: 2.5 (sumbu memanjang 2.5x radius)
- **speed**: Variable per frame (scaling speed)

### Five Circle Pattern Configuration

Pola lingkaran didasarkan pada **polar coordinates (angle + distance)** terhadap lingkaran utama:

```cpp
// Circle A di tengah (0, 0) - angle=0, distance=0
circleA = CircleShape(radius, "A", 0, 0);

// Circle B di kanan (0°) - angle=0, distance=radius
circleB = CircleShape(radius, "B", 0, radius);

// Circle C di kiri (180°) - angle=PI, distance=radius
circleC = CircleShape(radius, "C", PI, radius);

// Circle D di atas (90°) - angle=PI/2, distance=radius
circleD = CircleShape(radius, "D", PI/2, radius);

// Circle E di bawah (270°) - angle=-PI/2, distance=radius
circleE = CircleShape(radius, "E", -PI/2, radius);
```

**Draw Position Calculation:**
```cpp
// Hitung posisi dari angle dan distance saat draw
float x = cos(angle) * distance;
float y = sin(angle) * distance;
ofTranslate(x, y);
```

### Quadratic Bezier Curve untuk CustomLine

CustomLine menggunakan **Quadratic Bezier Curve** untuk membuat garis melengkung:

```cpp
// Rumus Quadratic Bezier: (1-t)²p0 + 2(1-t)t*p1 + t²p2
vec2 point = start * (1-t) * (1-t) +           // (1-t)²·P0 (weight ke start)
             controlPoint * 2 * (1-t) * t +    // 2(1-t)t·P1 (weight ke control)
             end * t * t;                      // t²·P2 (weight ke end)
```

**Parameter Curve:**
- **curve = 0**: Garis lurus (control point di tengah garis)
- **curve > 0**: Melengkung satu arah (control point bergeser perpendicular)
- **curve < 0**: Melengkung arah sebaliknya
- **Sampling**: 100 segments untuk smooth curve
- **Progress t**: 0.0 (start) → 1.0 (end)

### Runtime Radius Updates & Proportional Scaling

Semua shapes support **runtime radius updates** dengan sistem **proportional scaling**:

```cpp
void Shape::setRadius(float r) {
    // Hitung scaleFactor berdasarkan originalRadius
    float scaleFactor = r / originalRadius;

    // Re-calculate semua posisi secara proporsional
    start = start * scaleFactor;
    end = end * scaleFactor;
    intersecPoint = intersecPoint * scaleFactor;

    // Update radius dan originalRadius untuk scaling berikutnya
    radius = r;
    originalRadius = r;
}
```

**CircleShape - Angle/Distance Approach:**

CircleShape menggunakan **polar coordinates** (angle + distance) untuk positioning yang lebih scalable:

```cpp
// Constructor - simpan angle dan distance
CircleShape::CircleShape(float r, string label, float angle, float distance) {
    this->radius = r;
    this->originalRadius = r;  // Save untuk proportional scaling
    this->angle = angle;        // 0, PI/2, PI, -PI/2 (cardinal directions)
    this->distance = distance;  // 0 untuk Circle A, radius untuk lainnya
}

// Draw - hitung posisi dari angle dan distance
void CircleShape::draw() {
    float x = cos(angle) * distance;
    float y = sin(angle) * distance;
    ofTranslate(x, y);
    // ... draw circle dengan segment-based animation
}

// setRadius - re-calculate distance secara proporsional
void CircleShape::setRadius(float r) {
    float scaleFactor = r / originalRadius;
    distance = distance * scaleFactor;
    radius = r;
    originalRadius = r;
}
```

### Undo/Redo System Architecture

```cpp
// 100-step history dengan full state tracking
static const int MAX_UNDO_STEPS = 100;
std::vector<UndoAction> undoStack;
std::vector<UndoAction> redoStack;

struct UndoAction {
    UndoActionType type;  // CREATE_LINE, CREATE_POLYGON, dll
    std::vector<int> indices;  // Multi-select support
    std::vector<ofColor> oldColors;
    ofColor newColor;
    CustomLine deletedLine;
    PolygonShape deletedPolygon;
    // ... dll
};

void undo() {
    if (undoStack.empty()) return;
    UndoAction action = undoStack.back();
    undoStack.pop_back();

    // Restore state berdasarkan action type
    // Push modified action ke redoStack
}

void redo() {
    if (redoStack.empty()) return;
    UndoAction action = redoStack.back();
    redoStack.pop_back();

    // Execute redo
    // Push ke undoStack (tanpa clear redoStack)
}
```

**Supported Actions:**
- CREATE_LINE / CREATE_POLYGON
- CHANGE_COLOR_LINE / CHANGE_COLOR_POLYGON (multi-select)
- DELETE_LINE / DELETE_POLYGON
- CHANGE_CURVE (multi-select)

---

### Workspace Save/Load Format (.nay)

Workspace disimpan dalam **binary format** `.nay` dengan struktur berikut:

```cpp
// Header (64 bytes)
char magic[4] = "NA01";  // Magic number + version
int version = 2;         // Version number
char reserved[56];       // Reserved for future use

// Template Name (variable length)
int templateNameLength;
char* templateName;  // "Basic Zellige"

// Settings (10 bytes)
float radius;         // 4 bytes
float lineWidth;      // 4 bytes
bool labelsVisible;   // 1 byte
bool dotsVisible;     // 1 byte

// Custom Lines
int numCustomLines;   // 4 bytes
for (int i = 0; i < numCustomLines; i++) {
    vec2 start;        // 8 bytes (2 floats) - NORMALIZED positions
    vec2 end;          // 8 bytes (2 floats) - NORMALIZED positions
    float curve;       // 4 bytes
    ofColor color;     // 4 bytes (RGBA)
    float lineWidth;   // 4 bytes
    bool selected;     // 1 byte
}

// Polygons
int numPolygons;      // 4 bytes
for (int i = 0; i < numPolygons; i++) {
    int numVertices;   // 4 bytes
    for (int j = 0; j < numVertices; j++) {
        vec2 vertex;   // 8 bytes (2 floats) - NORMALIZED positions
    }
    ofColor color;     // 4 bytes (RGBA)
    float targetAlpha; // 4 bytes
    int animationMode; // 4 bytes (0=None, 1=FadeIn, 2=Wobble, 3=Fill)
    bool selected;     // 1 byte
}
```

**Key Features:**
- **NORMALIZED Positions**: Semua posisi dibagi radius saat save, dikali radius saat load
- **Versioning**: Support untuk backward compatibility
- **Reserved Space**: 56 bytes untuk future expansion
- **Complete State**: Template, settings, lines, polygons semuanya tersimpan

**Load Process:**

1. **Parallel Load (CTRL+O)**:
   - Baca file .nay
   - Setup template shapes dengan animasi parallel
   - Load custom lines dengan fade-in animation
   - Load polygons dengan animation (FadeIn/Wobble/Fill)
   - Semua animate sekaligus per group

2. **Sequential Load (CTRL+SHIFT+O)**:
   - Baca file .nay
   - Setup template shapes SEKARANG (tanpa animasi)
   - Load custom lines satu per satu dengan animasi
   - Load polygons satu per satu dengan animasi
   - Buffer system untuk smooth sequential loading

---

## 📁 Project Structure

```
BasicIslamicGeometry/
├── src/
│   ├── main.cpp              # Entry point aplikasi (1920x1080, OpenGL 4.6)
│   ├── ofApp.cpp/h           # Main application class (~2278 lines)
│   ├── shape/                # Shape implementations
│   │   ├── AbstractShape.cpp/h         # Base class untuk semua shapes
│   │   ├── CircleShape.cpp/h           # Circle dengan angle/distance positioning
│   │   ├── CartesianAxes.cpp/h         # X-Y axes dengan scaling animation
│   │   ├── CrossLine.cpp/h             # Diagonal lines dengan proportional scaling
│   │   ├── ParallelogramLine.cpp/h     # Connecting lines dengan intersections
│   │   ├── RectangleLine.cpp/h         # Rectangle dengan 2 intersection dots
│   │   ├── OctagramLine.cpp/h          # 2-phase animation lines
│   │   ├── CustomLine.cpp/h            # User-created bezier lines
│   │   ├── PolygonShape.cpp/h          # Fill-only polygons dengan animations
│   │   └── DotInfo.h                   # Common struct untuk dot information
│   ├── anim/                 # Animation system
│   │   ├── AbstractAnimation.cpp/h    # Base class untuk animations
│   │   ├── FadeInAnimation.cpp/h      # Alpha fade effect
│   │   ├── WobbleAnimation.cpp/h      # Oscillation effect
│   │   └── FillAnimation.cpp/h        # Water fill effect
│   ├── template/             # Template system
│   │   ├── SacredGeometryTemplate.cpp/h  # Abstract template base
│   │   ├── TemplateRegistry.cpp/h        # Singleton registry
│   │   └── templates/
│   │       └── BasicZelligeTemplate.cpp/h # Moroccan pattern (26 shapes)
│   └── operation/            # Operations layer
│       ├── FileManager.cpp/h       # .nay save/load
│       └── gui/                    # ImGui components
│           ├── AbstractGuiComponent.cpp/h # GUI base
│           ├── MenuBar.cpp/h            # File/Edit/View menus
│           ├── SacredGeometry.cpp/h     # Template control panel
│           ├── Playground.cpp/h         # Playback panel
│           ├── SuccessPopup.cpp/h       # Success dialog
│           └── ErrorPopup.cpp/h         # Error dialog
├── bin/                      # Compiled executable
│   └── data/                 # Saved workspaces (.nay files)
├── README.md                 # Comprehensive documentation (this file)
└── BasicIslamicGeometry.sln  # Visual Studio solution
```

**Total Files**: 64 source files (.cpp + .h)

**Architecture Highlights:**
- **Template System**: SacredGeometryTemplate base class untuk extensibility
- **Template Registry**: Singleton pattern untuk managing patterns
- **GUI System**: Modular ImGui components dengan AbstractGuiComponent
- **Shape Hierarchy**: Semua shapes inherit dari AbstractShape
- **Animation System**: AbstractAnimation base untuk reusable animations
- **Undo/Redo**: 100-step history dengan comprehensive state tracking
- **File Operations**: Centralized FileManager dengan direct file save
- **Window Management**: Independent window visibility controls

---

## 🎯 Purpose

Project ini adalah bagian dari eksplorasi **Creative Coding** dan pembelajaran:

- 🔬 Eksperimen geometric patterns dan Islamic art
- 🎨 Belajar animasi drawing dengan segment-based approach
- ⚡ Implementasi smooth animations dengan progress-based rendering
- 📚 Memahami arsitektur modular untuk geometric visualizations
- 🌿 Fondasi untuk project Islamic geometric patterns yang lebih kompleks
- 💾 Workspace persistence untuk save/load creative work
- 🎛️ Professional GUI development dengan ImGui

---

## 🚀 Performance

Dengan optimasi C++ modern dan openFrameworks:

- **Solid 60 FPS** pada resolusi bervariasi (1920x1080 default)
- **Smooth drawing animation** tanpa lag
- **Anti-aliased rendering** untuk kualitas visual tinggi
- **CPU-based rendering** (ideal untuk basic geometric shapes)
- **Lazy caching** untuk dot position queries
- **Smart pointer optimization** untuk memory management

---

## 📝 Current Status: **sketch-islamic-gs-modular-imgui**

Branch ini adalah **Islamic Geometry Studio with Modular Template System & GUI Playground** - aplikasi komprehensif untuk membuat, mengedit, dan menyimpan pola geometri Islam dengan GUI berbasis ImGui, sistem template yang modular, dan playground panel untuk playback workspace.

### ✨ Key Features

✅ **Complete ImGui Integration** - Full GUI dengan MenuBar, panels, popups
✅ **Modular Template System** - Easy pattern extensibility dengan SacredGeometryTemplate
✅ **100-Step Undo/Redo** - Complete history dengan multi-select support
✅ **Color Picker System** - Real-time color change untuk lines dan polygons
✅ **Independent Window Management** - Show/focus/close windows secara independen
✅ **Advanced Animation System** - FadeIn, Wobble, Fill animations untuk polygons
✅ **Direct File Operations** - Save langsung ke target tanpa intermediate files
✅ **Polygon Animation Modes** - Multiple animation types untuk visual variety
✅ **Custom Line Labels** - "CustomLine0", "CustomLine1", dst saat selected
✅ **Playground Auto-Focus** - Otomatis muncul saat file berhasil dibuka
✅ **Multi-Select Support** - CTRL+Click untuk multiple selection
✅ **Comprehensive Shortcuts** - 30+ keyboard shortcuts untuk power users

---

## 🤝 Contributing

Feel free to share your experiments!

---

## 📄 License

This project is licensed under the **Apache License 2.0** - see the LICENSE file for details.

---

## 🙏 Credits & References

- **OpenFrameworks** - Creative coding framework
- **[SandyKurt - Free Tutorials](https://sandykurt.com/free-tutorials)** - Islamic geometric patterns tutorials
- **[ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI library oleh Omar Cornut
- **Islamic Geometric Patterns Community** - Inspirasi dan resources
- **Creative Coding Community** - Inspirasi dan resources

---

## 🔗 Links

- **[OpenFrameworks](https://openframeworks.cc/)** - openframeworks.cc
- **[Watch Demo](https://youtu.be/_VGDcTeWVa0)** - YouTube demonstration
- **[SandyKurt Tutorials](https://sandykurt.com/free-tutorials)** - Free Islamic geometric patterns tutorials
- **[Support Me](https://sociabuzz.com/abdkdhni)** - Fund the experiments ☕

---

**Made with ❤️ for learning and creative exploration**
