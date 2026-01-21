## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch--octagram-orange)

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

- **Five Circle Pattern** - 5 lingkaran dengan radius dinamis yang saling berhubungan
- **CrossLine System** - 4 garis diagonal dari center ke sudut dengan dot di intersection point
- **Parallelogram Lines** - 4 garis penghubung antar circle center membentuk diamond/rhombus
- **Rectangle Lines** - 4 garis pembentuk rectangle (F→G, G→I, I→H, H→F) dengan 2 dot di intersection
- **Octagram Lines** - 8 garis pembentuk pola octagram (8-point star) dengan 2 segment per line (main + extension)
- **Polar Thinking** - Perhitungan posisi menggunakan trigonometri (cos, sin, atan2) untuk scalability
- **Sequential Drawing Mode** - Animasi drawing berurutan dari satu shape ke shape berikutnya
- **Animated Drawing** - Semua shape digambar dengan animasi smooth (100 segments, 0.5 speed)
- **Bidirectional Animation** - Animasi muncul (show) dan hilang (hide) dengan smoothing
- **Two-Phase Animation** - OctagramLine memiliki 2 mode: sequential (Phase 1 → Phase 2) dan paralel (barengan)
- **Cartesian Coordinate System** - Sumbu X-Y dengan animasi scaling (0 to 2.5x radius) dan label sudut (radians & degrees)
- **Dynamic Line Width** - Ketebalan garis yang dapat diadjust secara realtime (0.5px - 4px)
- **Static Font System** - Semua label menggunakan font normal (Calibri 15px) untuk konsistensi visual
- **Angle Labels** - Label sudut pada CartesianAxes menampilkan format "radians (degrees)" di setiap ujung sumbu
- **Label Toggle** - Show/hide label untuk semua shapes
- **Dot Toggle** - Show/hide dot di intersection points
- **Individual Controls** - Toggle CartesianAxes terpisah dari shapes lain
- **Trigonometry-Based Positioning** - Perhitungan posisi dot menggunakan atan2(), cos(), sin()
- **Scalable Intersection Points** - Semua intersection dihitung dengan rumus geometric yang scalable terhadap radiusCircle
- **AbstractShape Base Class** - Arsitektur OOP dengan inheritance untuk code reusability
- **Anti-Aliasing & Smoothing** - Garis yang smooth untuk visual yang lebih baik
- **Trails Effect** - Semi-transparent overlay untuk efek jejak visual yang menarik
- **Modular Design** - Terpisah dalam kategori: `shape/`

---

## 🎮 Controls

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

- **[OpenFrameworks 0.12.1](https://openframeworks.cc/)**
- **C++17**
- **Visual Studio 2022 Community**
- **Geometric Construction Algorithms** untuk Islamic patterns

---

## 📦 Installation

### Prerequisites

1. Install **[OpenFrameworks](https://openframeworks.cc/)** untuk Windows
2. Install **Visual Studio 2022** dengan workload "Desktop development with C++"

### Setup

```bash
# Clone repository ini
git clone https://github.com/username/BasicIslamicGeometry.git

# Checkout branch sketch-rectangle
git checkout sketch-rectangle

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

### Angle Labels pada CartesianAxes

CartesianAxes menampilkan label sudut di setiap ujung sumbu dengan format **radians (degrees)**:

```cpp
// Label sudut di 4 ujung sumbu
fontNormal.drawString("0 (0°)", currentLength + 10, 5);              // Kanan
fontNormal.drawString("PI/2 (90°)", -90, currentLength - 100);        // Bawah
fontNormal.drawString("PI (180°)", -currentLength - 80, 5);          // Kiri
fontNormal.drawString("-PI/2 (270°)", -100, -currentLength + 100);   // Atas
```

**Screen Coordinates Reference:**
- 0° (0 rad) = Timur (kanan) = X positif
- 90° (PI/2) = Selatan (bawah) = Y positif
- 180° (PI) = Barat (kiri) = X negatif
- 270° (-PI/2) = Utara (atas) = Y negatif

### Polar Thinking untuk RectangleLine

RectangleLine menggunakan **polar thinking** untuk perhitungan posisi dan intersection yang scalable:

```cpp
// Hitung posisi F dan G dengan polar (SCALABLE!)
float angleF = -3 * PI / 4;  // -135°
vec2 posF = vec2(cos(angleF) * radiusCircle, sin(angleF) * radiusCircle);

float angleG = -PI / 4;  // -45°
vec2 posG = vec2(cos(angleG) * radiusCircle, sin(angleG) * radiusCircle);

// Hitung intersection dengan rumus geometric yang scalable
vec2 intersecR = vec2(-radiusCircle * (1 - sqrt(2) / 2), posF.y);  // F→G ∩ C→E
vec2 intersecS = vec2(radiusCircle * (1 - sqrt(2) / 2), posF.y);   // F→G ∩ B→E
```

**Keuntungan Polar Thinking:**
- **Scalable**: Semua posisi otomatis menyesuaikan jika radiusCircle berubah
- **Geometric Intuitive**: Menggunakan angle dan distance sesuai nature Islamic geometry
- **Rotation-Friendly**: Mudah di-rotate dengan menambah offset angle
- **Compass-Based**: Sesuai dengan compass construction technique

**4 RectangleLine Configuration:**
- **F→G**: Horizontal di y = -170, dengan intersection R (-70, -170) dan S (70, -170)
- **G→I**: Vertical di x = 170, dengan intersection T (170, -70) dan U (170, 70)
- **I→H**: Horizontal di y = 170, dengan intersection V (70, 170) dan W (-70, 170)
- **H→F**: Vertical di x = -170, dengan intersection X (-170, 70) dan Y (-170, -70)

---

## 📁 Project Structure

```
BasicIslamicGeometry/
├── src/
│   ├── main.cpp              # Entry point aplikasi
│   ├── ofApp.cpp/h           # Main application class
│   └── shape/                # Shape implementations
│       ├── AbstractShape.cpp/h         # Base class untuk semua shapes
│       ├── CircleShape.cpp/h           # Circle dengan animasi drawing
│       ├── CartesianAxes.cpp/h         # Sumbu X-Y dengan scaling & angle labels
│       ├── CrossLine.cpp/h             # Garis diagonal dengan 2 dot & 2 label
│       ├── ParallelogramLine.cpp/h     # Garis penghubung dengan 1 dot & 1 label
│       ├── RectangleLine.cpp/h         # Garis rectangle dengan 2 dot & 2 label
│       └── OctagramLine.cpp/h          # Garis octagram dengan 2 segment & 1 dot (0-7)
├── bin/                      # Compiled executable
├── dll/                      # OF dependencies
├── obj/                      # Intermediate files (gitignored)
└── BasicIslamicGeometry.sln/.vcxproj # Visual Studio project files
```

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

## 📝 Current Status: **sketch-octagram**

Branch ini adalah **pengembangan** BasicIslamicGeometry dengan fokus pada **octagram lines** yang membentuk pola 8-point star dalam geometri Islam. Fitur yang tersedia:

✅ **Five Circle Pattern**: 5 lingkaran (A, B, C, D, E) dengan konfigurasi posisi yang saling berhubungan
✅ **Animated Drawing**: Lingkaran digambar dengan animasi smooth (100 segments, 0.5 speed)
✅ **Cartesian Axes**: Sistem koordinat X-Y dengan animasi scaling (0 to 2.5x radius) dan label sudut (radians & degrees)
✅ **CrossLine System**: 4 garis diagonal dari center ke sudut dengan dot di intersection
✅ **Parallelogram Lines**: 4 garis penghubung antar circle center dengan dot di intersection
✅ **Rectangle Lines**: 4 garis pembentuk rectangle (F→G, G→I, I→H, H→F) dengan 2 dot di intersection
✅ **Octagram Lines**: 8 garis pembentuk pola octagram (0, 1, 2, 3, 4, 5, 6, 7) dengan 2 segment per line
✅ **Polar Thinking**: Perhitungan posisi dan intersection menggunakan trigonometri untuk scalability
✅ **Scalable Intersections**: Semua intersection point dihitung dengan rumus geometric yang scalable terhadap radiusCircle
✅ **Bidirectional Animation**: Animasi muncul (show) dan hilang (hide) dengan smoothing
✅ **Two-Phase Animation System**: OctagramLine memiliki 2 mode animation (sequential & paralel)
✅ **Dynamic Line Width**: Ketebalan garis yang dapat diadjust (0.5px - 4px)
✅ **Static Font System**: Semua label menggunakan font normal (Calibri 15px) untuk konsistensi visual
✅ **Angle Labels**: Label sudut CartesianAxes dengan format "radians (degrees)" di setiap ujung sumbu
✅ **Show/Hide Controls**: Toggle visibility untuk semua shapes
✅ **Label System**: Label untuk semua shapes (Circle, CrossLine, Parallelogram, Rectangle, OctagramLine)
✅ **Dot System**: Dot di intersection points dengan toggle visibility
✅ **Trails Effect**: Semi-transparent overlay untuk efek jejak visual yang menarik
✅ **Anti-aliased Rendering**: Garis yang smooth untuk kualitas visual tinggi
✅ **Memory-safe Implementation**: Menggunakan `std::unique_ptr` untuk resource management
✅ **Modular Setup Methods**: setup() terbagi menjadi setupCircles(), setupCartesianAxes(), setupCrossLines(), setupParallelograms(), setupRectangleLine(), setupOctagramLine()

### Configuration:

**Circle Parameters:**
- **radius**: 240px untuk semua lingkaran
- **circleSpeed**: 0.5 segments per frame
- **totalSegments**: 100 segments untuk full circle
- **lineWidth**: 0.5px (thin) - 4px (thick)

**Circle Positions:**
- **Circle A**: (0, 0) - Center
- **Circle B**: (240, 0) - Right
- **Circle C**: (-240, 0) - Left
- **Circle D**: (0, 240) - Bottom
- **Circle E**: (0, -240) - Top

**Cartesian Axes Parameters:**
- **maxScale**: 2.5 (sumbu memanjang 2.5x radius = 600px)
- **speed**: 0.02 per frame
- **lineWidth**: 2px (default)
- **Angle Labels**: Format "radians (degrees)" di setiap ujung sumbu
  - Kanan: "0 (0°)"
  - Bawah: "PI/2 (90°)"
  - Kiri: "PI (180°)"
  - Atas: "-PI/2 (270°)"

**Font System:**
- **All Labels**: Calibri 15px (Regular)
- **Consistency**: Font tidak berubah saat line width di-adjust
- **Clean & Simple**: Visual yang konsisten di seluruh aplikasi

🎨 **Creative Freedom**: Project ini terbuka untuk eksplorasi dan improvisasi tanpa batas. Seni digital adalah tentang ekspresi, bukan checklist.

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
