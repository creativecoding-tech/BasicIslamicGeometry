## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch--islamic--geometry--studio-orange)

[![Fund The Experiments](https://img.shields.io/badge/Fund-The_Experiments-FF5722?style=for-the-badge&logo=buy-me-a-coffee)](https://sociabuzz.com/abdkdhni)

---

## 📺 Demo Video

Lihat hasilnya di YouTube: [Watch Demo](https://youtu.be/fr0wx18GXeI)

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
- **MenuBar** - Top menu bar dengan File menu (Save/Load workspace)
- **LeftPanel** - Side panel dengan settings (radius slider, line width, toggles)
- **SuccessPopup** - Modal popup untuk save/load confirmations
- **Template Name Display** - Menampilkan nama template saat ini di panel
- **Realtime Controls** - Semua settings apply secara realtime tanpa restart

### Animation System
- **Sequential Drawing Mode** - Animasi drawing berurutan dari satu shape ke shape berikutnya
- **Parallel Drawing Mode** - Animasi drawing parallel untuk semua shapes sekaligus
- **Animated Drawing** - Semua shape digambar dengan animasi smooth (100 segments, 0.5 speed)
- **Bidirectional Animation** - Animasi muncul (show) dan hilang (hide) dengan smoothing
- **Two-Phase Animation** - OctagramLine memiliki 2 mode: sequential (Phase 1 → Phase 2) dan paralel (barengan)
- **FadeInAnimation** - Animation fade-in untuk polygon dengan alpha blending (0 → targetAlpha)
- **Animation System** - Base class `AbstractAnimation` untuk reusable animations

### Custom Lines & Polygons
- **Mouse Interaction** - Mouse drag untuk menggambar line secara interaktif antar dots
- **CustomLine System** - Garis custom yang bisa dibuat user dengan mouse drag (start dot → end dot)
- **Bezier Curve Support** - Garis bisa dibuat melengkung dengan scroll mouse (curve parameter)
- **Multi-Select System** - CTRL+Klik untuk toggle selection multiple garis
- **PolygonShape System** - Class untuk polygon fill-only tanpa outline (hanya warna)
- **Create Polygon (CTRL+G)** - Buat polygon dari selected customLines (otomatis deteksi closed loop)
- **Polygon Color Preset** - 9 warna preset untuk polygon (merah, hijau, biru, kuning, magenta, cyan, orange, ungu, abu-abu)
- **Undo System** - CTRL+Z untuk undo garis terakhir

### Workspace Save/Load
- **Centralized Save/Load** - Satu method `saveWorkspace()` dan `loadWorkspace()` untuk semua state
- **.nay Binary Format** - Workspace format dengan magic number "NA01" dan versioning
- **Complete State Persistence** - Save template name, radius, custom lines, polygons, semua settings
- **Two Load Modes**:
  - **Parallel (CTRL+O)**: Semua shapes animate simultaneously
  - **Sequential (CTRL+SHIFT+O)**: Shapes animate satu per satu (template → lines → polygons)
- **Staggered Loading** - 3-stage loading process dengan buffer system
- **Animation State Preservation** - State animasi di-save dan di-restore dengan benar

### Performance & Rendering
- **Performance Optimization - Cached Dots** - Sistem lazy cache untuk getAllDots() dengan dirty flag, hanya rebuild saat visibility berubah (reduce dari 180 vector copies/detik menjadi 0)
- **Anti-Aliasing & Smoothing** - Garis yang smooth untuk visual yang lebih baik
- **60 FPS Performance** - Solid 60 FPS pada resolusi bervariasi dengan CPU-based rendering
- **Trails Effect** - Semi-transparent overlay untuk efek jejak visual yang menarik

### Display & Controls
- **Cartesian Coordinate System** - Sumbu X-Y dengan animasi scaling (0 to 2.5x radius) dan label sudut (radians & degrees)
- **Dynamic Line Width** - Ketebalan garis yang dapat diadjust secara realtime (0.5px - 4px)
- **Static Font System** - Semua label menggunakan font normal (Calibri 15px) untuk konsistensi visual
- **Angle Labels** - Label sudut pada CartesianAxes menampilkan format "radians (degrees)" di setiap ujung sumbu
- **Label Toggle** - Show/hide label untuk semua shapes
- **Dot Toggle** - Show/hide dot di intersection points
- **Individual Controls** - Toggle CartesianAxes terpisah dari shapes lain
- **Keyboard Shortcuts** - Comprehensive keyboard controls untuk semua operations

### Architecture & Code Quality
- **AbstractShape Base Class** - Arsitektur OOP dengan inheritance untuk code reusability
- **Template Method Pattern** - `SacredGeometryTemplate` untuk extensibility geometric patterns
- **Smart Pointer Management** - Menggunakan `std::unique_ptr` untuk resource management
- **Modular Design** - Terpisah dalam kategori: `shape/`, `anim/`, `operation/`, `template/`, `operation/gui/`
- **Memory-safe Implementation** - Modern C++17 features dengan RAII dan smart pointers

---

## 🎮 Controls

### GUI Controls (ImGui Panel)

**Left Panel Settings:**
| Control | Action |
| --- | --- |
| **Radius Slider** | Adjust template radius (100 - 240) secara realtime - semua shapes update posisinya secara proporsional |
| **Line Width Slider** | Adjust ketebalan garis (0 - 4px) untuk semua shapes |
| **Show Labels Checkbox** | Toggle visibility untuk semua label shapes |
| **Show Dots Checkbox** | Toggle visibility untuk semua dots di intersection points |

**MenuBar (File Menu):**
| Menu Item | Action | Keyboard Shortcut |
| --- | --- | --- |
| **Save Workspace** | Simpan semua state (template, radius, custom lines, polygons, settings) ke .nay file | **CTRL+S** |
| **Load Workspace (Parallel)** | Load workspace dengan animasi parallel (semua shapes barengan) | **CTRL+O** |
| **Load Workspace (Sequential)** | Load workspace dengan animasi sequential (shapes satu per satu) | **CTRL+SHIFT+O** |

**Success Popup:**
- Muncul setelah save/load berhasil
- Menampilkan pesan konfirmasi
- Auto-close setelah 2 detik atau klik anywhere

---

### Keyboard Controls

**Main Canvas Controls:**
| Input | Action |
| --- | --- |
| **SHIFT + 1** atau **SHIFT + !** | Sequential drawing - shapes muncul berurutan (CartesianAxes → Circle A-E → CrossLine F-I → Parallelogram N-Q → Rectangle R-Y → OctagramLine 0-7) |
| **SHIFT + )** | Show semua shapes (Circle, CrossLine, Parallelogram, Rectangle, OctagramLine, CartesianAxes) |
| **DEL** | Hide semua shapes (termasuk CartesianAxes) |
| **BACKSPACE** | Toggle CartesianAxes saja (hide/show) |
| **\`** atau **~** | Toggle label visibility (semua label) |
| **.** atau **>** | Toggle dot visibility (semua dot di intersection points) |
| **+** atau **=** | Increase line width (+0.5px, max 4px) |
| **-** atau **_** | Decrease line width (-0.5px, min 0px) |
| **END** | Keluar dari aplikasi |

**CustomLine Controls:**
| Input | Action |
| --- | --- |
| **Mouse Drag** | Buat garis custom dari dot ke dot (hanya bisa mulai dan akhir di dot) |
| **Mouse Click** | Select garis custom (berubah jadi merah) |
| **CTRL + Click** | Toggle selection multiple garis (multi-select) |
| **SHIFT + B** | Select semua customLines sekaligus |
| **Mouse Scroll** | Adjust curvature garis yang sedang di-select (scroll up = melengkung satu arah, scroll down = melengkung arah sebaliknya, 0 = lurus) |
| **CTRL + Z** | Undo garis terakhir yang dibuat |
| **CTRL + G** | Buat polygon dari selected customLines (minimum 3 garis untuk buat polygon, otomatis ikuti curve shape) |
| **CTRL + S** | Simpan semua custom lines dan polygons ke file binary (custom_lines.bin + polygons.bin) |
| **CTRL + O** | Load semua custom lines dan polygons dengan animasi parallel (barengan dengan fade-in) |
| **CTRL + SHIFT + O** | Load semua custom lines dan polygons dengan animasi sequential (lines dulu, baru polygons satu per satu) |
| **CTRL + DEL** | Hapus semua polygons dan custom lines (hanya jika animasi sudah selesai, tidak bisa saat sedang loading) |
| **BACKSPACE** | Hapus polygon/line yang sedang di-select (jika tidak ada yang selected dan sedang sequential load, cancel load. Jika tidak ada yang selected dan tidak sedang loading, toggle CartesianAxes) |
| **1 - 9** | Assign color ke polygon yang sedang di-select (1=Merah, 2=Hijau, 3=Biru, 4=Kuning, 5=Magenta, 6=Cyan, 7=Orange, 8=Ungu, 9=Abu-abu) |

**Polygon Controls:**
| Input | Action |
| --- | --- |
| **Mouse Click** | Select polygon (muncul label "Polygon0", "Polygon1", dst di tengah polygon) |
| **1 - 9** | Ganti warna polygon yang sedang di-select secara realtime |
| **CTRL + DEL** | Hapus semua polygons dan custom lines (hanya jika animasi sudah selesai) |
| **BACKSPACE** | Hapus polygon yang sedang di-select (hanya jika animasi sudah selesai) |

**Shape Count:**
- 5 CircleShape (A, B, C, D, E)
- 1 CartesianAxes
- 4 CrossLine (F, G, H, I) dengan 2 dot per line
- 4 ParallelogramLine (N, O, P, Q) dengan 1 dot per line
- 4 RectangleLine (F→G, G→I, I→H, H→F) dengan 2 dot per line
- 8 OctagramLine (0, 1, 2, 3, 4, 5, 6, 7) dengan 1 dot di end point
- **Total: 26 shapes**

---

## 🛠️ Tech Stack

- **[OpenFrameworks 0.12.1](https://openframeworks.cc/)** - Creative coding framework
- **[ImGui](https://github.com/ocornut/imgui)** - Immediate mode GUI library
- **[ImGui for OpenFrameworks](https://github.com/jvcleave/ofxImGui)** - ImGui integration
- **C++17** - Modern C++ features
- **Visual Studio 2022 Community** - IDE dan compiler
- **OpenGL 4.6** - Rendering backend
- **Geometric Construction Algorithms** - Islamic pattern mathematics

---

## 📦 Installation

### Prerequisites

1. Install **[OpenFrameworks](https://openframeworks.cc/)** untuk Windows
2. Install **Visual Studio 2022** dengan workload "Desktop development with C++"

### Setup Project

```bash
# Clone repository ini
git clone https://github.com/creativecoding-tech/BasicIslamicGeometry.git

# Checkout branch sketch-islamic-geometry-studio
git checkout sketch-islamic-geometry-studio

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
float angle = ofMap(i, 0, totalSegments, 0, TWO_PI);

// Hitung posisi X, Y untuk setiap segment
float x = cos(angle) * radius;
float y = sin(angle) * radius;
```

**Parameter Animation:**
- **totalSegments**: 100 segments untuk full circle
- **circleSpeed**: 0.5 segments per frame (drawing speed)
- **lineWidth**: 0.5px (tipis) sampai 4px (tebal)

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
- **speed**: 0.02 per frame (scaling speed)

### Five Circle Pattern Configuration

Pola lingkaran didasarkan pada **posisi relatif** terhadap lingkaran utama:

```cpp
// Circle A di tengah (0, 0)
circleA = CircleShape(radius, "A", 0, 0);

// Circle B di kanan (radius, 0) - sentuh Circle A di 0°
circleB = CircleShape(radius, "B", radius, 0);

// Circle C di kiri (-radius, 0) - sentuh Circle A di 180°
circleC = CircleShape(radius, "C", -radius, 0);

// Circle D di bawah (0, radius) - sentuh Circle A di 90°
circleD = CircleShape(radius, "D", 0, radius);

// Circle E di atas (0, -radius) - sentuh Circle A di 270°
circleE = CircleShape(radius, "E", 0, -radius);
```

### Static Font System

Semua label menggunakan **font normal** untuk konsistensi visual:

```cpp
// Semua label pakai font normal
fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);
```

**Font Configuration:**
- **All labels**: Calibri 15px (Regular)
- **Consistency**: Font tidak berubah saat line width di-adjust
- **Clean & Simple**: Visual yang konsisten di seluruh aplikasi

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

### Scalable Dot Positions System

Semua dot positions (intersection points) sekarang **fully scalable** dan diambil langsung dari shape objects, **TANPA hardcoded values**. Ini memastikan ketika `radiusCircle` berubah, semua posisi dot otomatis menyesuaikan.

---

### Runtime Radius Updates & Proportional Scaling

Semua shapes sekarang support **runtime radius updates** dengan sistem **proportional scaling**:

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
    // ... draw circle
}

// setRadius - re-calculate distance secara proporsional
void CircleShape::setRadius(float r) {
    float scaleFactor = r / originalRadius;
    distance = distance * scaleFactor;
    radius = r;
    originalRadius = r;
}
```

**Implementasi per Shape:**

- **CircleShape**: `angle` dan `distance` di-scale secara proporsional
- **CrossLine**: `end` dan `radiusDot` di-scale
- **ParallelogramLine**: `start`, `end`, `intersecCrossLine` di-scale
- **RectangleLine**: `start`, `end`, `intersec1`, `intersec2` di-scale
- **OctagramLine**: `start`, `end`, `nextPoint` (optional) di-scale
- **CartesianAxes**: Radius langsung di-update di draw()

**Dot Cache Auto-Rebuild:**

Setiap kali radius berubah via slider, dot cache otomatis di-rebuild:

```cpp
// Di ofApp.cpp - saat radius slider berubah
for (auto &shape : templateShapes) {
    shape->setRadius(radiusCircle);
    shape->update();
}

// Rebuild dot cache agar mouse hover/drag akurat
dotsCacheDirty = true;
```

**Keuntungan:**
- ✅ Semua shapes update posisinya secara realtime saat slider diubah
- ✅ Proporsi geometric tetap terjaga (tidak ada shape yang melenceng)
- ✅ Mouse hover/drag detection tetap akurat setelah radius berubah
- ✅ Tidak perlu rebuild template - cukup panggil `setRadius()`

---

### Workspace Save/Load Format (.nay)

Workspace disimpan dalam **binary format** `.nay` dengan struktur berikut:

```cpp
// Header (4 bytes)
char magic[4] = "NA01";  // Magic number + version

// Template Name (variable length)
int templateNameLength;
char* templateName;  // "Basic Zellige"

// Radius (4 bytes)
float radius;  // 240.0f

// Custom Lines Count (4 bytes)
int numCustomLines;

// Untuk setiap CustomLine:
for (int i = 0; i < numCustomLines; i++) {
    vec2 start;           // 8 bytes (2 floats)
    vec2 end;             // 8 bytes (2 floats)
    float curve;          // 4 bytes
    bool selected;        // 1 byte
}

// Polygons Count (4 bytes)
int numPolygons;

// Untuk setiap Polygon:
for (int i = 0; i < numPolygons; i++) {
    int numVertices;      // 4 bytes
    for (int j = 0; j < numVertices; j++) {
        vec2 vertex;      // 8 bytes (2 floats)
    }
    ofColor color;        // 4 bytes (RGBA)
    float targetAlpha;    // 4 bytes
    bool selected;        // 1 byte
}

// Settings (12 bytes total)
float lineWidth;         // 4 bytes
bool labelsVisible;      // 1 byte
bool dotsVisible;        // 1 byte
```

**Load Process (Two Modes):**

1. **Parallel Load (CTRL+O)**:
   - Baca file .nay
   - Setup template shapes dengan animasi parallel
   - Load custom lines dengan fade-in animation
   - Load polygons dengan fade-in animation
   - Semua animate sekaligus

2. **Sequential Load (CTRL+SHIFT+O)**:
   - Baca file .nay
   - Setup template shapes SEKARANG (tanpa animasi)
   - Load custom lines satu per satu dengan animasi
   - Load polygons satu per satu dengan animasi
   - Buffer system untuk smooth sequential loading

**File Manager Methods:**

```cpp
// Save workspace
void FileManager::saveAll(
    string templateName,
    float radius,
    vector<CustomLine>& lines,
    vector<PolygonShape>& polygons,
    float lineWidth,
    bool labelsVisible,
    bool dotsVisible
);

// Load workspace (parallel)
void FileManager::loadAllParallel(
    vector<CustomLine>& lines,
    vector<PolygonShape>& polygons
);

// Load workspace (sequential dengan buffer)
void FileManager::loadAllSequential(
    vector<CustomLine>& lines,
    vector<PolygonShape>& polygons,
    int& sequentialIndex,
    float& loadTimer
);
```

**Centralized Save/Load:**

Di ofApp, semua save/load operations di-centralize:

```cpp
void ofApp::saveWorkspace() {
    if (currentTemplate) {
        fileManager.saveAll(
            currentTemplate->getName(),
            radiusCircle,
            customLines,
            polygonShapes,
            currentLineWidth,
            labelsVisible,
            dotsVisible
        );
        successPopup->show();
    }
}

void ofApp::loadWorkspace(bool sequential) {
    // Load settings
    LoadedData data = fileManager.loadAll();

    // Apply settings
    radiusCircle = data.radius;
    currentLineWidth = data.lineWidth;
    labelsVisible = data.labelsVisible;
    dotsVisible = data.dotsVisible;

    // Switch template dan setup shapes
    switchTemplate(data.templateName);

    // Load lines & polygons dengan animation
    if (sequential) {
        startSequentialLoad(data);
    } else {
        startParallelLoad(data);
    }
}
```

---

## 📁 Project Structure

```
BasicIslamicGeometry/
├── src/
│   ├── main.cpp              # Entry point aplikasi
│   ├── ofApp.cpp/h           # Main application class (ImGui integration, main loop)
│   ├── shape/                # Shape implementations
│   │   ├── AbstractShape.cpp/h         # Base class untuk semua shapes
│   │   ├── CircleShape.cpp/h           # Circle dengan animasi drawing & angle/distance positioning
│   │   ├── CartesianAxes.cpp/h         # Sumbu X-Y dengan scaling & angle labels
│   │   ├── CrossLine.cpp/h             # Garis diagonal dengan proportional scaling
│   │   ├── ParallelogramLine.cpp/h     # Garis penghubung dengan proportional scaling
│   │   ├── RectangleLine.cpp/h         # Garis rectangle dengan 2 dot & proportional scaling
│   │   ├── OctagramLine.cpp/h          # Garis octagram dengan 2-phase animation
│   │   ├── CustomLine.cpp/h            # Custom user-created lines dengan bezier curve
│   │   └── PolygonShape.cpp/h          # Polygon fill-only dengan fade-in animation
│   ├── anim/                 # Animation system implementations
│   │   ├── AbstractAnimation.cpp/h    # Base class untuk semua animation
│   │   └── FadeInAnimation.cpp/h      # Fade-in animation untuk polygon
│   ├── template/             # Template system untuk geometric patterns
│   │   ├── SacredGeometryTemplate.cpp/h  # Abstract base class untuk templates
│   │   ├── SacredGeometryTemplateRegistry.cpp/h  # Singleton registry untuk templates
│   │   └── templates/              # Template implementations
│   │       └── BasicZelligeTemplate.cpp/h  # Moroccan pattern (26 shapes)
│   └── operation/            # File operations & GUI components
│       ├── gui/              # ImGui GUI components
│       │   ├── AbstractGuiComponent.cpp/h  # Base class untuk GUI components
│       │   ├── MenuBar.cpp/h            # Top menu bar (File menu)
│       │   ├── LeftPanel.cpp/h          # Settings panel (sliders, toggles)
│       │   └── SuccessPopup.cpp/h       # Modal popup untuk confirmations
│       └── FileManager.cpp/h           # Save/load workspace (.nay format)
├── bin/                      # Compiled executable
│   └── data/                 # Data files (saved workspaces .nay)
├── dll/                      # OF dependencies
├── obj/                      # Intermediate files (gitignored)
└── BasicIslamicGeometry.sln/.vcxproj # Visual Studio project files
```

**Architecture Highlights:**
- **Template System**: `SacredGeometryTemplate` base class untuk easy pattern extensibility
- **GUI System**: Modular ImGui components dengan `AbstractGuiComponent` base class
- **Shape Hierarchy**: Semua shapes inherit dari `AbstractShape` dengan consistent interface
- **Operation Layer**: Terpisah antara file operations (FileManager) dan GUI components
- **Polymorphic Design**: Template pattern, Strategy pattern, Registry pattern untuk scalability

---

## 🎯 Purpose

Project ini adalah bagian dari eksplorasi **Creative Coding** dan pembelajaran:

- 🔬 Eksperimen geometric patterns dan Islamic art
- 🎨 Belajar animasi drawing dengan segment-based approach
- ⚡ Implementasi smooth animations dengan progress-based rendering
- 📚 Memahami arsitektur modular untuk geometric visualizations
- 🌿 Fondasi untuk project Islamic geometric patterns yang lebih kompleks

---

## 🚀 Performance

Dengan optimasi C++ modern dan openFrameworks:

- **Solid 60 FPS** pada resolusi bervariasi
- **Smooth drawing animation** tanpa lag
- **Anti-aliased rendering** untuk kualitas visual tinggi
- **CPU-based rendering** (ideal untuk basic geometric shapes)

---

## 📝 Current Status: **sketch-islamic-geometry-studio**

Branch ini adalah **Islamic Geometry Studio** - aplikasi komprehensif untuk membuat, mengedit, dan menyimpan pola geometri Islam dengan GUI berbasis ImGui dan sistem template yang modular.

### 🎨 GUI System (ImGui Integration)

✅ **Complete ImGui Integration**: Full ImGui dengan OpenGL3 dan Win32 backends
✅ **AbstractGuiComponent Base Class**: Reusable base class untuk GUI components
✅ **MenuBar Component**: Top menu bar dengan File menu (Save/Load workspace)
✅ **LeftPanel Component**: Side panel dengan radius slider, line width, dan toggles
✅ **SuccessPopup Component**: Modal popup untuk save/load confirmations
✅ **Template Name Display**: Menampilkan nama template saat ini di panel
✅ **Realtime Updates**: Semua GUI controls apply secara realtime tanpa restart

### 🔄 Template System

✅ **SacredGeometryTemplate Base Class**: Abstract base class untuk geometric pattern templates
✅ **Template Registry**: Singleton pattern untuk managing multiple patterns
✅ **BasicZelligeTemplate**: Moroccan Islamic geometric pattern dengan 8-fold symmetry (26 shapes)
✅ **Modular Setup Methods**: setupCircles(), setupCartesianAxes(), setupCrossLines(), setupParallelograms(), setupRectangleLines(), setupOctagramLines()
✅ **Extensible Design**: Mudah menambahkan pattern baru dengan inherit dari SacredGeometryTemplate

### ⚡ Runtime Updates & Scalability

✅ **Runtime Radius Updates**: Semua 26 shapes update posisinya secara realtime saat radius slider diubah
✅ **Proportional Scaling System**: Posisi di-calculate ulang dengan `scaleFactor = newRadius / originalRadius`
✅ **Angle/Distance Approach**: CircleShape menggunakan polar coordinates (angle + distance) untuk better scaling
✅ **Cached Dot Auto-Rebuild**: Dot cache otomatis di-rebuild saat radius berubah untuk akurat mouse detection
✅ **CrossLine Runtime Update**: End position dan radiusDot scale secara proporsional
✅ **ParallelogramLine Runtime Update**: Start, end, dan intersection point scale secara proporsional
✅ **RectangleLine Runtime Update**: Start, end, dan kedua intersection points scale secara proporsional
✅ **OctagramLine Runtime Update**: Start, end, dan nextPoint scale secara proporsional

### 💾 Workspace Save/Load System

✅ **Centralized Save/Load**: Satu method `saveWorkspace()` dan `loadWorkspace()` untuk semua state
✅ **.nay Binary Format**: Workspace format dengan magic number "NA01" dan versioning
✅ **Complete State Persistence**: Save template name, radius, custom lines, polygons, semua settings (labels, dots, line width)
✅ **Parallel Load Mode (CTRL+O)**: Load workspace dengan animasi parallel (semua shapes barengan)
✅ **Sequential Load Mode (CTRL+SHIFT+O)**: Load workspace dengan animasi sequential (shapes satu per satu)
✅ **Staggered Loading**: 3-stage loading process (template → custom lines → polygons)
✅ **Animation State Preservation**: State animasi di-save dan di-restore dengan benar
✅ **Speed Control**: Configurable speed untuk sequential load animation
✅ **File Menu Integration**: Save/Load dapat diakses via ImGui MenuBar atau keyboard shortcuts

### 🖱️ Mouse Interaction & Custom Lines

✅ **Mouse Drag System**: Buat garis custom secara interaktif antar dots
✅ **CustomLine Class**: Encapsulated class dengan curve support, progressive animation, dan selection state
✅ **Bezier Curve Support**: Garis bisa dibuat melengkung dengan mouse scroll (curve parameter)
✅ **Multi-Select System**: CTRL+Klik untuk toggle selection multiple garis
✅ **SHIFT+B Shortcut**: Select semua customLines sekaligus
✅ **Line Selection**: Klik garis untuk select, visual feedback dengan warna merah
✅ **Curve Label**: Display nilai curve saat garis di-select
✅ **Undo System (CTRL+Z)**: Undo garis terakhir yang dibuat
✅ **Invalid Line Filter**: Filter garis yang start == end (panjang 0)

### 🎭 Polygon System

✅ **PolygonShape Class**: Encapsulated class untuk polygon fill dengan color preset system
✅ **Create Polygon (CTRL+G)**: Buat polygon dari selected customLines (otomatis deteksi closed loop)
✅ **Polygon Selection**: Klik polygon untuk select, visual feedback dengan label "Polygon0", "Polygon1", dst
✅ **9 Color Presets**: Merah, hijau, biru, kuning, magenta, cyan, orange, ungu, abu-abu
✅ **Color Assignment (Keys 1-9)**: Assign color ke selected polygon secara realtime
✅ **Fade-In Animation**: Polygon muncul perlahan dari transparan ke warna asli saat load
✅ **Delete Protection**: Tidak bisa delete saat animasi polygon belum selesai

### 🎬 Animation System

✅ **AbstractAnimation Base Class**: Base class untuk reusable animations
✅ **FadeInAnimation**: Fade-in untuk polygon dengan alpha blending (0 → targetAlpha)
✅ **Segment-Based Drawing**: Semua shapes digambar dengan 100 segments untuk smoothness
✅ **Bidirectional Animation**: Animasi muncul (show) dan hilang (hide) dengan smoothing
✅ **Two-Phase Animation**: OctagramLine memiliki 2 mode (sequential & paralel)
✅ **Sequential Mode**: Shapes muncul berurutan (CartesianAxes → Circle A-E → CrossLine F-I → Parallelogram N-Q → Rectangle R-Y → OctagramLine 0-7)
✅ **Parallel Mode**: Semua shapes animate sekaligus
✅ **Configurable Speed**: Animation speed dapat di-adjust (0.5 segments/frame default)

### 📐 Geometric Patterns

✅ **Five Circle Pattern**: 5 lingkaran (A, B, C, D, E) dengan konfigurasi posisi kardinal
✅ **Cartesian Axes**: Sistem koordinat X-Y dengan animasi scaling (0 to 2.5x radius) dan label sudut
✅ **CrossLine System**: 4 garis diagonal dari center ke sudut dengan dot di intersection
✅ **Parallelogram Lines**: 4 garis penghubung antar circle center dengan dot di intersection
✅ **Rectangle Lines**: 4 garis pembentuk rectangle dengan 2 dot di intersection
✅ **Octagram Lines**: 8 garis pembentuk pola octagram dengan 2 segment per line
✅ **Polar Thinking**: Perhitungan posisi menggunakan trigonometri (cos, sin, atan2)
✅ **Scalable Intersections**: Semua intersection dihitung dengan rumus geometric yang scalable

### 🎨 Display & Rendering

✅ **Dynamic Line Width**: Ketebalan garis dapat di-adjust (0.5px - 4px) via GUI atau keyboard
✅ **Static Font System**: Semua label menggunakan Calibri 15px untuk konsistensi
✅ **Angle Labels**: Format "radians (degrees)" di setiap ujung sumbu Cartesian
✅ **Label Toggle**: Show/hide label untuk semua shapes via GUI atau keyboard
✅ **Dot Toggle**: Show/hide dot di intersection points via GUI or keyboard
✅ **Trails Effect**: Semi-transparent overlay untuk efek jejak visual
✅ **Anti-aliased Rendering**: Garis smooth untuk kualitas visual tinggi
✅ **60 FPS Performance**: Solid performance dengan CPU-based rendering

### ⚙️ Architecture & Code Quality

✅ **AbstractShape Base Class**: OOP inheritance untuk code reusability
✅ **Memory-safe Implementation**: `std::unique_ptr` untuk resource management
✅ **Modular Design**: Terpisah dalam `shape/`, `anim/`, `operation/`, `template/`, `operation/gui/`
✅ **Template Pattern**: SacredGeometryTemplate untuk extensibility geometric patterns
✅ **Strategy Pattern**: Different loading modes (parallel/sequential)
✅ **Registry Pattern**: Template registry untuk managing multiple patterns
✅ **Modern C++17**: RAII, smart pointers, move semantics
✅ **Comprehensive Documentation**: Well-documented codebase

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
- **Islamic Geometric Patterns Community** - Inspirasi dan resources
- **Creative Coding Community** - Inspirasi dan resources

---

## 🔗 Links

- **[OpenFrameworks](https://openframeworks.cc/)** - openframeworks.cc
- **[Watch Demo](https://youtu.be/fr0wx18GXeI)** - YouTube demonstration
- **[SandyKurt Tutorials](https://sandykurt.com/free-tutorials)** - Free Islamic geometric patterns tutorials
- **[Support Me](https://sociabuzz.com/abdkdhni)** - Fund the experiments ☕

---

**Made with ❤️ for learning and creative exploration**
