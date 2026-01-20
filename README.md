## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch--parallelogram-orange)

[![Fund The Experiments](https://img.shields.io/badge/Fund-The_Experiments-FF5722?style=for-the-badge&logo=buy-me-a-coffee)](https://sociabuzz.com/abdkdhni)

---

## 📺 Demo Video

Lihat hasilnya di YouTube: [Watch Demo](https://youtu.be/fr0wx18GXeI)

---

## 📺 Preview

Project ini menampilkan **pola geometri Islam dasar** dengan lima lingkaran yang saling berhubungan, crosslines, dan parallelogram lines:

- **5 CircleShape (A, B, C, D, E)** - Lingkaran dengan posisi kardinal
- **Cartesian Axes** - Sistem koordinat X-Y untuk referensi posisi
- **4 CrossLine (F, G, H, I)** - Garis diagonal dari center ke sudut dengan dot di intersection
- **4 ParallelogramLine (N, O, P, Q)** - Garis penghubung antar circle center dengan dot di intersection

Setiap shape memiliki **animasi drawing** yang halus, label yang dinamis, dot di posisi intersection, dan line width yang dapat diadjust.

---

## ✨ Fitur & Teknik

- **Five Circle Pattern** - 5 lingkaran dengan radius dinamis yang saling berhubungan
- **CrossLine System** - 4 garis diagonal dari center ke sudut dengan dot di intersection point
- **Parallelogram Lines** - 4 garis penghubung antar circle center membentuk diamond/rhombus
- **Sequential Drawing Mode** - Animasi drawing berurutan dari satu shape ke shape berikutnya
- **Animated Drawing** - Semua shape digambar dengan animasi smooth (100 segments, 0.5 speed)
- **Bidirectional Animation** - Animasi muncul (show) dan hilang (hide) dengan smoothing
- **Cartesian Coordinate System** - Sumbu X-Y dengan animasi scaling (0 to 2.5x radius)
- **Dynamic Line Width** - Ketebalan garis yang dapat diadjust secara realtime (0.5px - 4px)
- **Dynamic Font System** - Font yang berubah otomatis (bold/normal) berdasarkan line width
- **Label Toggle** - Show/hide label untuk semua shapes (A-E, F-M, N-Q)
- **Dot Toggle** - Show/hide dot di intersection points (17 dot total)
- **Individual Controls** - Toggle CartesianAxes terpisah dari shapes lain
- **Trigonometry-Based Positioning** - Perhitungan posisi dot menggunakan atan2(), cos(), sin()
- **AbstractShape Base Class** - Arsitektur OOP dengan inheritance untuk code reusability
- **Anti-Aliasing & Smoothing** - Garis yang smooth untuk visual yang lebih baik
- **Trails Effect** - Semi-transparent overlay untuk efek jejak visual yang menarik
- **Modular Design** - Terpisah dalam kategori: `shape/`

---

## 🎮 Controls

| Input | Action |
| --- | --- |
| **SHIFT + 1** atau **SHIFT + !** | Sequential drawing - shapes muncul berurutan (CartesianAxes → Circle A-E → CrossLine F-I → Parallelogram N-Q) |
| **SHIFT + )** | Show semua shapes (Circle, CrossLine, Parallelogram, CartesianAxes) |
| **DEL** | Hide semua shapes (termasuk CartesianAxes) |
| **BACKSPACE** | Toggle CartesianAxes saja (hide/show) |
| **\`** atau **~** | Toggle label visibility (semua label A-E, F-M, N-Q) |
| **.** atau **>** | Toggle dot visibility (17 dot di intersection points) |
| **+** atau **=** | Increase line width (+0.5px, max 4px, font jadi bold) |
| **-** atau **_** | Decrease line width (-0.5px, min 0px, font jadi normal) |
| **END** | Keluar dari aplikasi |

**Shape Count:**
- 5 CircleShape (A, B, C, D, E)
- 1 CartesianAxes
- 4 CrossLine (F, G, H, I) dengan 2 dot per line
- 4 ParallelogramLine (N, O, P, Q) dengan 1 dot per line
- **Total: 14 shapes dengan 17 dot**

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

# Checkout branch sketch-parallelogram
git checkout sketch-parallelogram

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

### Dynamic Font System

Font berubah otomatis berdasarkan **line width**:

```cpp
// Tipis (lineWidth < 2)
fontNormal.load("C:\\Windows\\Fonts\\calibri.ttf", 15);

// Tebal (lineWidth >= 2)
fontBold.load("C:\\Windows\\Fonts\\calibrib.ttf", 20);
```

**Rumus Mapping:**
- **Thin mode**: `lineWidth = 0.5px`, font = Calibri 15px (Regular)
- **Thick mode**: `lineWidth = 4px`, font = Calibri 20px (Bold)

---

## 📁 Project Structure

```
BasicIslamicGeometry/
├── src/
│   ├── main.cpp              # Entry point aplikasi
│   ├── ofApp.cpp/h           # Main application class
│   └── shape/                # Shape implementations
│       ├── CircleShape.cpp/h # Circle class dengan animasi drawing
│       └── CartesianAxes.cpp/h # Cartesian axes dengan animasi scaling
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

## 📝 Current Status: **sketch-parallelogram**

Branch ini adalah **pengembangan** BasicIslamicGeometry dengan fokus pada **parallelogram lines** yang membentuk pola diamond/rhombus dalam geometri Islam. Fitur yang tersedia:

✅ **Five Circle Pattern**: 5 lingkaran (A, B, C, D, E) dengan konfigurasi posisi yang saling berhubungan
✅ **Animated Drawing**: Lingkaran digambar dengan animasi smooth (100 segments, 0.5 speed)
✅ **Cartesian Axes**: Sistem koordinat X-Y dengan animasi scaling (0 to 2.5x radius)
✅ **Bidirectional Animation**: Animasi muncul (show) dan hilang (hide) dengan smoothing
✅ **Dynamic Line Width**: Ketebalan garis yang dapat diadjust (0.5px - 4px)
✅ **Dynamic Font System**: Font yang berubah otomatis (bold/normal) berdasarkan line width
✅ **Show/Hide Controls**: Toggle visibility untuk semua lingkaran dan axes
✅ **Label System**: Setiap lingkaran memiliki label (A, B, C, D, E) yang dinamis
✅ Trails effect untuk visual impact
✅ Anti-aliased rendering untuk kualitas visual tinggi
✅ Memory-safe implementation dengan `std::unique_ptr`

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
