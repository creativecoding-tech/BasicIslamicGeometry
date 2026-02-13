## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch--islamic--gs--playgroundhandle-orange)

[![Fund The Experiments](https://img.shields.io/badge/Fund-The_Experiments-FF5722?style=for-the-badge&logo=buy-me-a-coffee)](https://sociabuzz.com/abdkdhni)

---

## 📺 Demo Video

Lihat hasilnya di YouTube: [Watch Demo](https://youtu.be/A50uG7nty7U)

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

### Draw Only Concept ⭐ NEW
- **Playground Draw Settings** - Checkbox di Playground mengontrol shapes yang DIBUAT:
  - ✓ Dicentang = Shapes dibuat dan ditampilkan
  - ✗ Tidak dicentang = Shapes TIDAK dibuat sama sekali
- **No More Hide/Show** - Konsep show/hide sudah DIHAPUS sepenuhnya:
  - AbstractShape tidak punya `showing` flag
  - Tidak ada method `show()` dan `hide()`
  - Shapes selalu forward animation (progress: 0 → maxProgress)
- **Efficient Rendering** - Hanya shapes yang dicentang yang di-render, menghemat resources
- **Bug Fix for Circle A** - Circle A (di origin 0,0) dan CrossLine F sekarang muncul dengan benar ketika Cartesian tidak dicentang

### Delta Time Animation System
- **Time-Based Animation** - Semua animasi menggunakan deltaTime untuk consistency
  - Template shapes (Circle, CrossLine, dll) menggunakan `progress += speed * deltaTime`
  - Polygons menggunakan `animation->update(deltaTime)`
  - CustomLines menggunakan `updateProgress(deltaTime)`
- **Frame Rate Independent** - Animasi konsisten di berbagai frame rates (60 FPS, 120 FPS, dll)
- **Smooth Animation** - Tidak ada lagi "patah-patah" atau inconsistent speed
- **ofGetLastFrameTime()** - OpenFrameworks function untuk mendapatkan deltaTime

### Speed Control System
- **Global Speed Multiplier** - Slider speed 0.1 - 1.5 di SacredGeometry panel
  - 0.1 = Sangat lambat (10% dari normal speed)
  - 1.0 = Normal speed
  - 1.5 = 1.5x lebih cepat
- **Apply to ALL** - Speed multiplier berlaku untuk:
  - Template shapes (Circle, CrossLine, Parallelogram, dll)
  - Polygons (FadeIn, Wobble, Fill animations)
  - CustomLines (drawing animation)
- **Consistent Speed** - Saat load file .nay, speed mengikuti slider setting
- **No Speed Variation** - Tidak ada lagi animasi "cepat kedua, lambat ketiga" - selalu konsisten!

### FileOperationManager ⭐ NEW
- **Wrapper Pattern** - FileOperationManager class untuk semua file operations (save, open, load)
- **Clean Architecture** - Mengikuti pattern yang sama dengan ColorManager/DuplicateManager
- **Centralized File Operations** - Semua file operations dipindah dari ofApp ke FileOperationManager:
  - `saveWorkspace()` - Save ke lastSavedPath (CTRL+S)
  - `saveWorkspaceAs()` - Save dengan dialog (CTRL+SHIFT+S)
  - `openWorkspace()` - Buka file dialog untuk load workspace
  - `loadWorkspace()` - Parallel load dengan staggered animation
  - `loadWorkspaceSeq()` - Sequential load dengan per-group animation
- **Peek Functions** - Optimasi untuk membaca file header tanpa load full data:
  - `peekFileCustomLinesCount()` - Baca jumlah customLines dari file
  - `peekFilePolygonCount()` - Baca jumlah polygons dari file menggunakan sequential load buffer
- **Conditional UI Display** - CollapsingHeader hanya muncul jika file punya data:
  - Custom Line CollapsingHeader hanya muncul jika `loadedFileCustomLinesCount > 0`
  - Polygon CollapsingHeader hanya muncul jika `loadedFilePolygonCount > 0`

### Transform Canvas System ⭐ NEW
- **Canvas Transform Controls** - Transform slider di SacredGeometry panel:
  - **Pan X** - Geser canvas horizontal (-500 to +500 pixels)
  - **Pan Y** - Geser canvas vertikal (-500 to +500 pixels)
  - **Rotate** - Putar canvas (0° to 360°)
  - **Zoom** - Scale canvas (0.5x to 3.0x)
- **Transform Order** - Transform diaplikasikan dengan urutan:
  1. Translate ke center screen
  2. Rotate (dari center)
  3. Zoom/Scale (dari center)
  4. Pan/Translation (geser posisi)
- **Inverse Transform** - Mouse input menggunakan `screenToWorld()` untuk konversi screen coordinates ke world coordinates
- **Reset Transform Button** - Kembalikan transform ke default (no pan, no rotation, 1.0x zoom)
- **Transform Not Saved** - Transform state TIDAK disimpan ke file .nay (viewport-only)

### Draw Template System
- **SacredGeometry Panel Controls**:
  - **Draw Template** section dengan 2 opsi:
    - **Parallel** - Draw semua shapes secara parallel (barengan)
    - **Sequential** - Draw shapes satu per satu dengan animasi
  - **Clean Canvas** - Benar-benar hapus semua shapes (bukan hide)
- **Playground Draw Button** - Tombol Arrow (←) di Playground window:
  - Clean canvas dulu (hapus semua shapes, polygons, customLines)
  - Apply speed multiplier ke semua animations
  - Load workspace dengan animasi sesuai mode (Parallel/Sequential)
- **Load File** - Saat load/open file .nay (CTRL+O):
  - Template shapes dibuat (setupShapes) TAPI progress=0 (belum visible)
  - User perlu klik tombol Draw di Playground untuk meng-animate
  - Tombol Parallel/Sequential di SacredGeometry disabled jika shapes sudah dibuat
- **Shapes Always Visible** - Shapes yang sudah dibuat SELALU ditampilkan (tidak bisa di-hide)

### Selection Info Panel ⭐ NEW
- **Floating Window** - Menampilkan informasi lengkap tentang selected objects:
  - **Dots** - Radius, color, offset position
  - **Lines** - Curve value, color, lock state
  - **Polygons** - Vertex count, color
- **Multi-Line Indented Format** - Format tampilan yang mudah dibaca:
  ```
  dot[0]
    radius: 8.0
    color: rgba(0, 0, 255, 255)
    offset = 10.0
  ```
- **Center on First Open** - Window muncul di tengah screen saat pertama dibuka
- **View Menu Integration** - Bisa diakses via View → Selection Info

### Object Tooltip System ⭐ NEW
- **Hover Tooltips** - OpenFrameworks-based custom rendering untuk selected objects:
  - **Dots** - Offset dari dot parent + index dot
  - **Lines** - Label (customLine0/DcustomLine0) + curve value
  - **Polygons** - Index polygon + vertex count
- **Performance Optimized** - Direct OF rendering tanpa ImGui overhead
- **Positioned Near Object** - Tooltips muncul dekat object (dot/line midpoint/polygon centroid)
- **Only for Selected Objects** - Tooltips hanya muncul untuk objects yang terseleksi

### Draw Button Behavior (Playground)
- **Clean First, Then Draw** - Saat tombol Draw (Arrow ←) di Playground diklik:
  1. **Clean Canvas** - Hapus semua shapes, polygons, customLines, userDots + **reset settings** (L/P/D colors → blue, Dot → visible, Radius → 8.0f) ⭐ UPDATED
  2. **Apply Speed** - Sync speed multiplier ke FileManager untuk polygons & customLines
  3. **Set Animation Mode** - FadeIn/Wobble/Fill/None sesuai radio button
  4. **Load Workspace** - Load dari file .nay dengan animation (Parallel/Sequential mode)
- **Validation** - Button cek apakah file sudah dibuka dan mode sudah dipilih
- **Error Popup** - Muncul jika belum ada file atau belum pilih mode

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
  - **File Menu**: Save Workspace (CTRL+S), Save As (CTRL+SHIFT+S), Open (CTRL+O), Exit (END)
  - **Edit Menu**: Undo (CTRL+Z), Redo (CTRL+SHIFT+Z), Delete All Custom Lines, Delete All Polygons, Delete Lines & Polygons (CTRL+DEL), Clean Canvas (CTRL+SHIFT+DEL)
  - **View Menu**: Sacred Geometry, Playground, User Custom, Selection Info (show/focus windows independently) ⭐ UPDATED
- **SacredGeometry Panel** - Template controls panel dengan:
  - Template Name Display (Basic Zellige)
  - **Draw Template** section: Parallel / Sequential buttons
  - **Clean Canvas** button
  - Radius Slider (50-600) - real-time scaling
  - **Speed Control Slider (0.1 - 1.5)**
  - Line Width Slider (0-4px)
  - Labels, Dots checkboxes
  - **Transform Canvas Section**: Pan X/Y, Rotate, Zoom sliders, Reset Transform button ⭐ NEW
  - **Custom Line Color Picker** - Circular color wheel untuk custom line color (default: blue)
  - **Polygon Color Picker** - Circular color wheel untuk polygon color (default: blue)
  - Template-Specific Settings (collapsible)
- **Playground Panel** - Playback panel dengan:
  - Opened File Display
  - Mode Draw: Parallel Per Group / Sequential Per Group radio buttons
  - **Draw Settings**: Cartesian, Circles, CrossLines, Parallelograms, RectangleLines, OctagramLines checkboxes ⭐ UPDATED
  - Polygon Animate: No Animation / FadeIn / Wobble / Fill / Wobble Fill / Gradient radio buttons
  - Speed Control Slider (0.1 - 1.5) ⭐ NEW
  - **Draw Arrow Button (←)** - Load dan animate workspace
- **UserCustom Panel** - User control panel dengan:
  - **Dot Section**:
    - Show/Hide Checkbox
    - Radius Slider (0 - 8) untuk userDots (ambil langsung dari slider saat create) ⭐ UPDATED
    - Dot Color Picker (circular wheel)
    - **Color Sync** - Color picker sync dengan selected userDot atau global color
  - **Line Section**:
    - Selected Lines Info - Menampilkan label dan count selected lines
    - Line Color Picker (circular wheel with hue wheel & alpha bar)
    - Reset All Colors Button - Reset semua customLines ke default blue
    - Reset Selected Colors Button - Reset selected customLines ke default blue
    - **Duplicate Line R 180° Button** - Duplicate selected lines dengan rotate 180°
      - Hanya enabled jika ≥2 selected original lines
      - Validasi otomatis untuk mencegah duplicate dari duplicate
  - **Polygon Section**:
    - Polygon Color Picker (circular wheel with hue wheel & alpha bar)
    - Reset All/Selected Polygon Colors Buttons
    - Alpha Slider (0-255) untuk selected polygons (≥2 polygons)
- **SelectionInfo Panel** ⭐ NEW - Selected objects info window:
  - Multi-line indented format untuk readability
  - Shows dot/line/polygon properties
  - Auto-center on first open
- **Independent Window Management** - SacredGeometry, Playground, UserCustom, dan SelectionInfo windows bisa di-close (X button) dan di-show secara independen melalui View menu
- **SuccessPopup** - Modal popup untuk save/load success confirmations
- **ErrorPopup** - Modal popup untuk error handling
- **Realtime Controls** - Semua settings apply secara realtime tanpa restart
- **G Key Toggle** - Tekan G untuk toggle ImGui: MenuBar only → All windows → Hide all

### Animation System
- **Sequential Drawing Mode** - Animasi drawing berurutan dari satu shape ke shape berikutnya
- **Parallel Drawing Mode** - Animasi drawing parallel untuk semua shapes sekaligus
- **Animated Drawing** - Semua shape digambar dengan animasi smooth (100 segments, configurable speed)
- **Forward Only Animation** - Semua animasi hanya forward (0 → maxProgress), tidak ada reverse ⭐ UPDATED
- **Two-Phase Animation** - OctagramLine memiliki 2 mode: sequential (Phase 1 → Phase 2) dan paralel (barengan)
- **Delta Time Based** - Semua animasi menggunakan deltaTime untuk consistency
- **Polygon Animation System** - Base class `AbstractAnimation` untuk reusable animations:
  - **FadeInAnimation** - Alpha blending fade-in (0 → targetAlpha) dengan deltaTime
  - **WobbleAnimation** - Oscillation effect dengan amplitude dan frequency dengan deltaTime
  - **FillAnimation** - Directional fill (bottom to top) dengan wave effect dan deltaTime
  - **WobbleFillAnimation** - Kombinasi wobble + gradual fill dengan deltaTime ⭐ NEW
    - Polygon bergerak-goyang (wobble) SEKALIGUS di-fill secara gradual
    - Parameters: wobbleAmount, wobbleSpeed, wobbleFrequency, targetFillLevel, fillSpeed
    - getWobbleOffset() - Vertex offset untuk goyangan
    - getFillLevel() - Current fill level (0.0 - 1.0)
  - **GradientAnimation** - Smooth gradient flow effect dengan finite duration ⭐ NEW
    - Smooth gradient flow effect pada warna fill polygon
    - Finite animation: berhenti setelah duration (default 5 detik)
    - Parameters: speed, frequency, duration
    - getFrequency() - Spatial frequency gradient pattern
    - getDuration() - Durasi animation (detik)
  - **Configurable Speed** - Animation speed dapat di-adjust via speed slider

### Custom Lines & Polygons
- **Mouse Interaction** - Mouse drag untuk menggambar line secara interaktif antar dots
- **CustomLine System** - Garis custom yang bisa dibuat user dengan mouse drag (start dot → end dot)
- **Bezier Curve Support** - Garis bisa dibuat melengkung dengan scroll mouse (curve parameter)
- **Multi-Select System** - CTRL+Klik untuk toggle selection multiple garis
- **CustomLine Labels** - Saat selected, muncul label dari getLabel() (customLine0, DcustomLine0, dst)
- **Curve Label** - Display nilai curve saat garis di-select
- **Draw Custom Lines Checkbox** - Checkbox "Draw Custom Lines" mengontrol apakah customLines digambar saat Draw ⭐ NEW:
  - ✓ Dicentang = CustomLines diload dan digambar
  - ✗ Tidak dicentang = CustomLines TIDAK diload (di-skip saat load)
- **Skip CustomLines Load** - Parallel vs Sequential load behavior ⭐ NEW:
  - **Parallel load**: CustomLines diload, lalu di-clear jika flag false
  - **Sequential load**: CustomLines TIDAK ditambahkan ke buffer jika flag false
- **PolygonShape System** - Class untuk polygon fill-only tanpa outline (hanya warna)
- **Create Polygon (CTRL+G)** - Buat polygon dari selected customLines (otomatis deteksi closed loop)
- **Polygon Color Preset** - 9 warna preset untuk polygon (merah, hijau, biru, kuning, magenta, cyan, orange, ungu, abu-abu)
- **GLSL vs CPU Rendering** - Conditional rendering berdasarkan `loadedFromFile` flag ⭐ NEW:
  - **Loaded polygons** (dari .nay file) → Gunakan GLSL shaders untuk rendering dan animations
    - FadeIn: GLSL alpha blending dengan deltaTime
    - Wobble: GLSL offset animation dengan deltaTime
    - Wave Fill: Multi-pass FBO dengan wave shader
  - **New polygons** (CTRL+G / right-click) → Gunakan CPU rendering sederhana
    - Tidak ada animation
    - Direct ofSetColor() + ofBeginShape()
- **Animation System** - AbstractAnimation base class untuk reusable polygon animations:
  - **FadeInAnimation** - Alpha blending fade-in (0 → targetAlpha) dengan deltaTime
  - **WobbleAnimation** - Oscillation effect dengan amplitude dan frequency dengan deltaTime
  - **FillAnimation** - Water fill effect dari bawah ke atas dengan wave dan deltaTime
  - **Configurable Speed** - Animation speed di-adjust via speed slider (0.12f - 1.8f * multiplier)
  - **Different Base Speeds** - Setiap tipe animation punya base speed berbeda:
    - Wobble: 1.8f * animationSpeedMultiplier
    - FadeIn: 0.18f * animationSpeedMultiplier
    - Wave Fill: 0.12f * animationSpeedMultiplier
- **Update Strategy** - Polygons hanya di-update jika belum complete (isAnimationComplete() check) ⭐ NEW
  - Mencegah animations berhenti prematur
  - Berlaku untuk semua load modes (parallel dan sequential)
- **Color Picker Integration** - Ubah warna custom line dan polygon secara real-time via color picker di UserCustom panel
- **Color Sync System** - Color picker otomatis sync dengan warna object yang terseleksi:
  - Saat select object → color picker update ke warna object tersebut
  - Saat tidak ada selection → color picker tetap di warna terakhir
- **Paste Color Update** - Paste color sekarang mengupdate global color variables untuk consistency
- **100-Step Undo/Redo System** - Complete undo/redo dengan full state tracking:
  - CREATE_LINE / CREATE_POLYGON / CREATE_DOT
  - CHANGE_COLOR_LINE / CHANGE_COLOR_POLYGON (multi-select support)
  - DELETE_LINE / DELETE_POLYGON / DELETE_DOT
  - CHANGE_CURVE (multi-select support)
- **Delete All Custom Lines** - Hapus semua customLines saja (Edit menu)
- **Delete All Polygons** - Hapus semua polygons saja (Edit menu)

### UserDot System ⭐ NEW
- **Duplicate Dot Feature** - Buat duplicate dot dari original template dots:
  - **Duplicate Dot Above** - Duplicate dengan offset ke atas (Y negatif)
  - **Duplicate Dot Below** - Duplicate dengan offset ke bawah (Y positif)
  - **Duplicate Dot Left** - Duplicate dengan offset ke kiri (X negatif)
  - **Duplicate Dot Right** - Duplicate dengan offset ke kanan (X positif)
- **Context Menu Access** - Right-click pada original dot untuk show duplicate options
- **Radius from Slider** - New userDot radius diambil langsung dari slider User Custom (0-8) ⭐ UPDATED
  - Tidak lagi dipengaruhi lineWidth template
  - Consistent dengan user expectations
- **Color from Picker** - New userDot color diambil dari color picker User Custom
- **Lower Bound System** - Setiap userDot punya lower bound (dot parent position) untuk reference
- **Scroll Control** - Mouse scroll untuk menggerakkan selected userDots:
  - Horizontal dots (left/right) → Scroll gerakkan di sumbu X
  - Vertical dots (above/below) → Scroll gerakkan di sumbu Y
  - Dengan boundary validation (tidak bisa melewati dot parent)
- **Color Copy/Paste** - Copy color dari selected userDot, paste ke userDots lain
- **Undo/Redo Support** - CREATE_DOT action dengan radius preservation ⭐ UPDATED
  - Saat undo CREATE_DOT → Simpan radius dot yang dihapus
  - Saat redo CREATE_DOT → Buat ulang dengan radius yang tersimpan
  - Consistent radius saat undo/redo

### Duplicate Line System
- **Duplicate Line R 180°** - Duplicate selected customLines dengan rotasi 180° di global center
  - Hanya bisa duplicate original customLines (isDuplicate = false)
  - Minimal 2 selected lines untuk duplicate
  - Hasil duplicate langsung LOCK_BOTH (fully locked) untuk mencegah pergeseran tidak sengaja
  - Label otomatis dengan prefix "D" (customLine0 → DcustomLine0)
- **DcustomLine System** - Duplicate lines dengan isDuplicate flag dan axis lock system
  - **isDuplicate Field** - Flag untuk membedakan original vs duplicate line
  - **Persistent State** - isDuplicate dan axisLock tersimpan di .nay file
  - **Scroll Control** - Mouse scroll untuk menggerakkan DcustomLine sesuai axis lock
- **Axis Lock System** - Control pergerakan DcustomLine dengan 4 state:
  - **NONE** - Bisa gerak bebas di X dan Y (scroll serong/diagonal)
  - **LOCK_X** - Hanya bisa gerak di Y (atas bawah) via scroll
  - **LOCK_Y** - Hanya bisa gerak di X (kanan kiri) via scroll
  - **LOCK_BOTH** - Fully locked (tidak bisa gerak sama sekali)
- **Context Menu System** - Right-click context menu untuk DcustomLine:
  - **1 DcustomLine** → Per-line operation (Lock X/Y, Unlock X/Y)
  - **>1 DcustomLine** → Bulk operation (Lock All, Unlock All, Lock X/Y All, Unlock X/Y All)
  - **Dynamic Labels** - Menu label berubah sesuai state (Lock ↔ Unlock)
- **Scroll-Based Movement** - Mouse wheel untuk menggerakkan DcustomLine:
  - Scroll speed: 2.0f (sama dengan userDot)
  - Otomatis skip original customLines (hanya DcustomLine yang bisa di-scroll)
  - Priority: userDot > DcustomLine > curve adjustment

### Workspace Save/Load
- **FileOperationManager** - Wrapper class untuk semua file operations (mirip ColorManager/DuplicateManager pattern) ⭐ NEW
- **Centralized Save/Load** - Semua file operations dipindah ke FileOperationManager:
  - `saveWorkspace()` - Save ke lastSavedPath (CTRL+S)
  - `saveWorkspaceAs()` - Save dengan dialog (CTRL+SHIFT+S)
  - `openWorkspace()` - Buka file dialog untuk load workspace
  - `loadWorkspace()` - Parallel load dengan staggered animation
  - `loadWorkspaceSeq()` - Sequential load dengan per-group animation
- **.nay Binary Format** - Workspace format dengan magic number "NA01", version 2
- **Complete State Persistence** - Save template name, radius, custom lines, polygons, semua settings (labels, dots, line width, draw settings)
- **Direct File Save** - Save langsung ke filepath target tanpa intermediate "workspace.nay" file
- **File Dialog Integration** - Native Windows file dialog untuk Save As/Open operations dengan .nay filter validation
- **Draw Mode Selection** - Mode animate dipilih lewat radio button di Playground:
  - **Parallel Per Group**: Template → CustomLines → Polygons animate simultaneously per group
  - **Sequential Per Group**: Groups animate satu per satu dengan delay
- **Staggered Load Setup** - Parallel load (CTRL+O) menggunakan staggered load system ⭐ NEW:
  - LOAD_TEMPLATE → LOAD_CUSTOMLINES → LOAD_POLYGONS → LOAD_DONE
  - Template shapes dibuat progress=0, baru di-animate via Draw button
  - CustomLines dan polygons diload dengan animation sesuai mode
- **Sequential Load Setup** - Sequential load (CTRL+SHIFT+O) dengan per-group animation:
  - Template shapes dulu → CustomLines → Polygons
  - Setiap group tunggu sampai complete sebelum lanjut
- **Draw Custom Lines Integration** - "Draw Custom Lines" checkbox mengontrol customLines load ⭐ NEW:
  - Dicentang → CustomLines diload dan digambar
  - Tidak dicentang → CustomLines di-skip saat load
- **Auto Clean Canvas** - Otomatis bersihkan canvas sebelum load (selalu dicenterangkan)
- **Delay Load System** - Smooth transition dengan delay sebelum animation starts (0.0f = tanpa delay)
- **Animation State Preservation** - State animasi di-save dan di-restore dengan benar
- **Playground Auto-Focus** - Saat file berhasil dibuka, Playground window otomatis muncul dan focus
- **No Success Popup on Load** - Load operations tidak menampilkan success popup (hanya save yang menampilkan) ⭐ NEW
- **Error Handling** - Comprehensive error handling untuk no file selected, invalid format, canvas not empty, no mode selected
- **Color Picker Sync** - Color picker otomatis sync dengan warna customLines/polygons yang diload
- **Speed Sync** - Speed multiplier otomatis sync ke FileManager saat load

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
- **Draw Settings** - Checkbox di Playground mengontrol shapes yang dibuat (Cartesian, Circles, CrossLines, dll) ⭐ UPDATED
- **Comprehensive Keyboard Shortcuts** - 30+ keyboard shortcuts untuk power users

### Architecture & Code Quality
- **AbstractShape Base Class** - Arsitektur OOP dengan inheritance untuk code reusability
  - Tidak ada `showing` flag (Draw Only concept) ⭐ UPDATED
  - Tidak ada method `show()` dan `hide()` (dihapus) ⭐ UPDATED
  - Forward only animation (progress: 0 → maxProgress) ⭐ UPDATED
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
| **Draw Template - Parallel** | Setup & show semua shapes secara parallel (barengan) |
| **Draw Template - Sequential** | Setup & start sequential drawing animation (shapes muncul berurutan) |
| **Clean Canvas Button** | Clear semua polygons, custom lines, dan **hapus** template shapes + **reset settings** (L/P/D colors → blue, Dot checkbox → checked, Radius → 8.0f) ⭐ UPDATED |
| **Radius Slider** | Adjust template radius (50 - 600) secara realtime - semua shapes update posisinya secara proporsional |
| **Speed Slider** | Adjust global speed multiplier (0.1 - 1.5x) untuk semua animations |
| **Line Width Slider** | Adjust ketebalan garis (0 - 4px) untuk semua shapes |
| **Labels Checkbox** | Toggle visibility untuk semua label shapes |
| **Dots Checkbox** | Toggle visibility untuk semua dots di intersection points |
| **Line Color Picker** | Pilih warna untuk custom lines (circular color wheel with alpha bar) |
| **Polygon Color Picker** | Pilih warna untuk selected polygon (circular color wheel with alpha bar) |
| **Template-Specific** | Collapsible section untuk template-specific settings (BasicZellige has additional controls) |
| **Transform Canvas Section** ⭐ NEW | - |
| **Pan X Slider** | Geser canvas horizontal (-500 to +500 pixels) |
| **Pan Y Slider** | Geser canvas vertikal (-500 to +500 pixels) |
| **Rotate Slider** | Putar canvas (0° to 360°) |
| **Zoom Slider** | Scale canvas (0.5x to 3.0x) |
| **Reset Transform Button** | Reset transform ke default |

**Playground Panel:**
| Control | Action |
| --- | --- |
| **Opened File Display** | Menampilkan nama file .nay yang terakhir dibuka |
| **Mode Draw** | Pilihan mode animasi: |
|  - **Parallel Per Group** | Template, CustomLines, dan Polygons animate secara parallel per group |
|  - **Sequential Per Group** | Groups animate satu per satu dengan delay |
| **Custom Line CollapsingHeader** | ⭐ NEW Hanya muncul jika file punya customLines: |
|  - **Draw Custom Lines Checkbox** | CustomLines diload/digambar jika dicentang |
|  - **Custom Line Appearance** | CustomLine controls (color, curve, etc) |
| **Polygon CollapsingHeader** | ⭐ NEW Hanya muncul jika file punya polygons: |
|  - **Polygon Animate** | Pilihan animation mode untuk polygons: |
|  - **No Animation** | Polygons langsung muncul tanpa animasi |
|  - **FadeIn** | Alpha blending fade-in effect |
|  - **Wobble** | Oscillation/goyang effect |
|  - **Fill** | Water fill effect dari bawah ke atas |
|  - **Wobble Fill** | Kombinasi wobble + gradual fill ⭐ NEW |
|  - **Gradient** | Smooth gradient flow effect ⭐ NEW |
| **Draw Settings** ⭐ UPDATED | Shapes yang DIBUAT saat Draw diklik: |
|  - **Cartesian** Checkbox | Cartesian axes dibuat jika dicentang |
|  - **Circles** Checkbox | Circle shapes dibuat jika dicentang |
|  - **CrossLines** Checkbox | CrossLine shapes dibuat jika dicentang |
|  - **Parallelograms** Checkbox | ParallelogramLine shapes dibuat jika dicentang |
|  - **RectangleLines** Checkbox | RectangleLine shapes dibuat jika dicentang |
|  - **OctagramLines** Checkbox | OctagramLine shapes dibuat jika dicentang |
| **Speed Control** ⭐ NEW | Slider speed 0.1 - 1.5x untuk semua animations |
| **Draw Arrow Button (←)** | Load dan animate workspace |
   - Clean canvas dulu
  - Apply speed multiplier
  - Set polygon animation mode
  - Load workspace (langsung, tanpa delay) |

**UserCustom Panel:**
| Control | Action |
| --- | --- |
| **Dot Section** | - |
| **Show/Hide Checkbox** | Toggle userDot visibility |
| **Radius Slider** | Adjust radius userDots (0 - 8) ⭐ UPDATED - Applied ke selected userDots, new userDots ambil dari slider ini |
| **D Color Picker** | Pilih warna untuk userDots (circular wheel) - Sync dengan selected userDot ⭐ UPDATED |
| **Line Section** | - |
| **Selected Lines Info** | Menampilkan label dan jumlah selected lines (auto wrap) |
| **L Color Picker** | Pilih warna untuk selected lines (circular wheel with hue wheel & alpha bar) |
| **Reset All Colors** | Reset semua customLines ke default blue |
| **Reset Selected Colors** | Reset selected customLines ke default blue |
| **Duplicate Line R 180°** | Duplicate selected lines dengan rotate 180° di global center |
| **Polygon Section** | - |
| **Polygon Info** | Menampilkan label dan jumlah selected polygons (auto wrap) |
| **P Color Picker** | Pilih warna untuk polygons (circular wheel with hue wheel & alpha bar) |
| **Reset All Polygon Colors** | Reset semua polygons ke default blue |
| **Reset Selected Polygon Colors** | Reset selected polygons ke default blue |
| **Alpha Slider** | Adjust transparansi selected polygons (0-255, min 2 polygons) |

**SelectionInfo Panel:** ⭐ NEW
| Control | Action |
| --- | --- |
| **Selected Objects Info** | Menampilkan informasi lengkap selected objects: |
|  - **Dots** | Radius, color, offset position (multi-line indented format) |
|  - **Lines** | Curve value, color, lock state |
|  - **Polygons** | Vertex count, color |

**MenuBar (File Menu):**
| Menu Item | Action | Keyboard Shortcut |
| --- | --- | --- |
| **Save Workspace** | Simpan semua state ke lastSavedPath | **CTRL+S** |
| **Save As...** | Simpan workspace ke lokasi custom (file dialog) | **CTRL+SHIFT+S** |
| **Open...** | Buka file dialog untuk load workspace .nay | **CTRL+O** |
| **Exit** | Keluar dari aplikasi | **END** |

**MenuBar (Edit Menu):**
| Menu Item | Action | Keyboard Shortcut |
| --- | --- | --- |
| **Undo** | Undo last action | **CTRL+Z** |
| **Redo** | Redo last undone action | **CTRL+SHIFT+Z** |
| **Delete All Custom Lines** | Hapus semua custom lines saja | - |
| **Delete All Polygons** | Hapus semua polygons saja | - |
| **Delete Lines & Polygons** | Hapus semua custom lines dan polygons | **CTRL+DEL** |
| **Clean Canvas** | Clear semua, **hapus** template shapes, + **reset settings** (L/P/D colors → blue, Dot → visible, Radius → 8.0f) ⭐ UPDATED | **CTRL+SHIFT+DEL** |

**MenuBar (View Menu):** ⭐ UPDATED
| Menu Item | Action |
| --- | --- |
| **Sacred Geometry** | Show SacredGeometry window (hidden) atau focus ke window (already visible) |
| **Playground** | Show Playground window (hidden) atau focus ke window (already visible) |
| **User Custom** | Show UserCustom window (hidden) atau focus ke window (already visible) |
| **Selection Info** | Show SelectionInfo window (hidden) atau focus ke window (already visible) ⭐ NEW |

**Window Management:**
- **Close Button (X)** - Setiap window (SacredGeometry, Playground, UserCustom, SelectionInfo) punya tombol close di pojok kanan atas
- **Independent Visibility** - Window bisa di-close dan di-show secara independen
- **Auto-Focus** - Saat file berhasil dibuka, Playground window otomatis muncul dan focus
- **Auto-Center** - SelectionInfo window muncul di tengah screen saat pertama dibuka ⭐ NEW

**Success Popup:**
- Muncul setelah save berhasil ⭐ UPDATED (tidak muncul untuk load operations)
- Menampilkan pesan konfirmasi
- Auto-close setelah klik OK atau anywhere

**Error Popup:**
- Muncul saat ada error (invalid file format, no file selected, no mode selected)
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
| **DEL** | Tidak melakukan apa-apa (hide sudah tidak dipakai lagi) |
| **BACKSPACE** | Hapus selected line/polygon/userDot ⭐ UPDATED |
| **\`** atau **~** | Toggle label visibility (semua label) |
| **.** atau **>** | Toggle dot visibility (semua dot di intersection points) |
| **+** atau **=** | Increase line width (+0.5px, max 4px) |
| **-** atau **_** | Decrease line width (-0.5px, min 0px) |
| **END** | Keluar dari aplikasi |

**CustomLine Controls:**
| Input | Action |
| --- | --- |
| **Mouse Drag** | Buat garis custom dari dot ke dot (hanya bisa mulai dan akhir di dot) ⭐ UPDATED - Menggunakan inverse transform |
| **Mouse Click** | Select garis custom (muncul label dari getLabel()) |
| **CTRL + Click** | Toggle selection multiple garis (multi-select) |
| **SHIFT + B** | Select semua customLines sekaligus |
| **Mouse Scroll (Original)** | Adjust curvature garis yang selected (scroll up/down, 0 = lurus) |
| **Mouse Scroll (DcustomLine)** | Gerakkan DcustomLine sesuai axis lock state |
| **Right-Click (DcustomLine)** | Context menu untuk lock/unlock axis |
| **Duplicate Line R 180°** | Duplicate selected original lines (min 2 lines) |
| **1 - 9** | Assign color ke selected polygon (1=Merah, ..., 9=Abu-abu) |

**DcustomLine Context Menu Controls:**
| Menu (1 Selected) | Action |
| --- | --- |
| **Lock X Axis** | Lock X axis (hanya bisa gerak di Y via scroll) |
| **Unlock X Axis** | Unlock X axis (bisa gerak di X dan Y) |
| **Lock Y Axis** | Lock Y axis (hanya bisa gerak di X via scroll) |
| **Unlock Y Axis** | Unlock Y axis (bisa gerak di X dan Y) |

**UserDot Controls:** ⭐ NEW
| Input | Action |
| --- | --- |
| **Right-Click (Original Dot)** | Context menu untuk Duplicate Dot (Above/Below/Left/Right) |
| **Right-Click (UserDot)** | Context menu untuk Copy/Paste Color |
| **Mouse Scroll (Selected)** | Gerakkan selected userDot sesuai arah offset (without CTRL) |
| **Radius Slider** | Atur radius userDot (0-8) - applied ke selected userDots atau new userDots |
| **Color Picker** | Atur warna userDot - applied ke selected userDots atau new userDots |

**DcustomLine Bulk Menu Controls (>1 Selected):**
| Menu (>1 Selected) | Action |
| --- | --- |
| **Unlock All** | Set semua ke NONE (bisa gerak bebas di X dan Y) |
| **Lock All** | Set semua ke LOCK_BOTH (fully locked) |
| **Lock X All** | Set semua ke LOCK_X (hanya Y) |
| **Lock Y All** | Set semua ke LOCK_Y (hanya X) |
| **Unlock X All** | Unlock X axis untuk semua (LOCK_BOTH → LOCK_Y, LOCK_X → NONE) |
| **Unlock Y All** | Unlock Y axis untuk semua (LOCK_BOTH → LOCK_X, LOCK_Y → NONE) |

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
| **BACKSPACE** | Hapus selected polygon ⭐ UPDATED |

**File Operations:**
| Input | Action |
| --- | --- |
| **CTRL + S** | Save workspace ke lastSavedPath |
| **CTRL + SHIFT + S** | Save workspace ke custom location |
| **CTRL + O** | Buka file dialog untuk load workspace .nay |

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

# Checkout branch sketch-islamic-gs-playgroundhandle
git checkout sketch-islamic-gs-playgroundhandle

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

### Delta Time Animation System

Semua animasi sekarang menggunakan **delta time** untuk consistency:

```cpp
// Di ofApp.cpp - update loop
float deltaTime = ofGetLastFrameTime();
currentTemplate->update(deltaTime);

// Di Shape classes (CircleShape, CrossLine, dll)
void update(float deltaTime) {
    // Animasi muncul dari 0 ke totalSegments ⭐ UPDATED
    if (progress < totalSegments) {
        progress += speed * deltaTime;  // ← Delta time!
    }
}

// Speed calibration untuk delta time:
// Lama (frame-based): speed = 0.5f
// Baru (time-based): speed = 50.0f (0.5f × 60 FPS)
```

**Keuntungan:**
- Animasi konsisten di berbagai frame rates
- Tidak ada lagi "cepat kedua, lambat ketiga"
- Smooth animation di semua kondisi

### Transform Canvas System ⭐ NEW

Canvas transform diaplikasikan dengan urutan sebagai berikut:

```cpp
void ofApp::draw() {
    ofPushMatrix();

    // 1. Translate ke center screen
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

    // 2. Apply rotation (dari center)
    ofRotateDeg(canvasRotation);

    // 3. Apply zoom/scale (dari center)
    ofScale(canvasZoom, canvasZoom);

    // 4. Apply pan/translation (geser posisi canvas)
    ofTranslate(canvasTranslation.x, canvasTranslation.y);

    // Draw semua shapes...
    currentTemplate->draw();

    ofPopMatrix();
}
```

**Inverse Transform untuk Mouse Input:**

```cpp
vec2 ofApp::screenToWorld(vec2 screenPos) {
    vec2 pos = screenPos;

    // 1. Translate relative to center
    pos.x -= ofGetWidth() / 2.0f;
    pos.y -= ofGetHeight() / 2.0f;

    // 2. Inverse rotate (-rotation)
    float rotRad = ofDegToRad(-canvasRotation);
    float cosR = cos(rotRad);
    float sinR = sin(rotRad);
    float newX = pos.x * cosR - pos.y * sinR;
    float newY = pos.x * sinR + pos.y * cosR;
    pos.x = newX;
    pos.y = newY;

    // 3. Inverse scale (1/zoom)
    pos.x /= canvasZoom;
    pos.y /= canvasZoom;

    // 4. Inverse pan (-pan)
    pos.x -= canvasTranslation.x;
    pos.y -= canvasTranslation.y;

    return pos;
}
```

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
- **speed**: Variable speed per shape (drawing speed) - sekarang menggunakan deltaTime
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
- **speed**: Variable per frame (scaling speed) - sekarang menggunakan deltaTime

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
- **Animation speed**: Menggunakan deltaTime

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

### Speed Control System Architecture

```cpp
// SacredGeometryTemplate.h
class SacredGeometryTemplate {
    float speedMultiplier = 1.0f;  // Global speed multiplier

    void applySpeedMultiplier() {
        // Loop semua shapes dan update speed
        for (auto& shape : shapes) {
            // Base speed untuk setiap tipe shape
            if (dynamic_cast<CircleShape*>(shape.get())) {
                shape->speed = 50.0f * speedMultiplier;
            }
            // ... dll untuk shape lainnya
        }
    }
};

// FileManager - untuk polygons & customLines
class FileManager {
    float animationSpeedMultiplier;  // Dari SacredGeometry panel slider

    void createPolygonWithAnimation(...) {
        // Gunakan speed multiplier untuk animation
        float polySpeed;
        switch (polygonAnimationMode) {
            case FADE_IN:
                polySpeed = 0.18f * animationSpeedMultiplier;
                break;
            case WOBBLE:
                polySpeed = 1.8f * animationSpeedMultiplier;
                break;
            // ... dll
        }
        auto fadeIn = std::make_unique<FadeInAnimation>(..., polySpeed);
        // ...
    }
};
```

**Speed Calibration Table:**
| Component | Old Speed (Frame-based) | New Speed (Delta Time) | Calibration |
|-----------|---------------------|----------------------|-------------|
| Template Shapes (Circle, CrossLine, dll) | 0.5f | 50.0f | × 100 |
| CartesianAxes | 0.02f | 1.2f | × 60 |
| CustomLines (load) | 0.003f / 0.03f | 0.18f / 1.8f | × 60 |
| Polygons (FadeIn) | 0.003f | 0.18f | × 60 |
| Polygons (Wobble) | 0.03f | 1.8f | × 60 |
| Polygons (Fill) | 0.002f | 0.12f | × 60 |
| Polygons (Wobble Fill) | - | 0.3f | × 18 (estimated) ⭐ NEW |
| Polygons (Gradient) | - | 2.0f | × 4 ⭐ NEW |

### GLSL Polygon Rendering System ⭐ NEW

Polygon rendering menggunakan conditional rendering berdasarkan `loadedFromFile` flag:

```cpp
void PolygonShape::draw() const {
    if (loadedFromFile) {
        // Polygon diload dari file .nay → Gunakan GLSL shaders
        drawGLSL();
    } else {
        // Polygon dibuat baru (CTRL+G / right-click) → Gunakan CPU rendering
        drawCPU();
    }
}
```

**CPU Rendering (New Polygons):**
- Simple ofSetColor() + ofBeginShape() rendering
- Tidak ada animation
- Direct color fill

**GLSL Rendering (Loaded Polygons):**

1. **Basic Rendering (No Animation/FadeIn):**
   - Vertex shader untuk tessellated curve support
   - Fragment shader untuk solid color rendering
   - Alpha fade untuk FadeIn animation

2. **Wobble Rendering (WobbleAnimation):** ⭐ NEW
   - Vertex shader (wobble.vert) untuk wobble displacement
   - Fragment shader (wobble.frag) untuk solid color
   - Vertex offset berdasarkan getWobbleOffset() dari WobbleAnimation
   - Smooth oscillation effect pada polygon vertices

3. **Wave Fill Rendering (FillAnimation):**
   - Multi-pass rendering dengan FBO (mask + quad)
   - Mask FBO untuk polygon shape
   - Wave shader untuk water fill effect
   - Auto-switch to No Animation setelah complete (fix jagged edges bug) ⭐ UPDATED

4. **Wobble Fill Rendering (WobbleFillAnimation):**
   - Multi-pass rendering dengan FBO (mask + quad)
   - Mask FBO untuk polygon shape
   - Wobble + fill shader untuk kombinasi goyang + gradual fill
   - Auto-switch to No Animation setelah complete

5. **Gradient Rendering (GradientAnimation):**
   - Multi-pass rendering dengan FBO (mask + quad)
   - Mask FBO untuk polygon shape
   - Gradient shader untuk smooth color flow effect
   - Finite duration animation (auto-stop setelah selesai)
   - Auto-switch to No Animation setelah complete

6. **Animation Integration:**
   - Delta time untuk smooth animations
   - Progress-based rendering (0.0 → 1.0)
   - Complete check untuk auto-switch ke No Animation (FadeIn, Fill, WobbleFill, Gradient, Wobble) ⭐ UPDATED

**Update Strategy:**
```cpp
void ofApp::updateStaggeredPolygons() {
    float deltaTime = ofGetLastFrameTime();
    // Update animation polygons yang BELUM complete saja ⭐ BUG FIX
    for (auto& polygon : polygonShapes) {
        if (!polygon.isAnimationComplete()) {
            polygon.update(deltaTime);
        }
    }
}

void ofApp::updatePolygons() {
    float deltaTime = ofGetLastFrameTime();
    // Update polygons yang BELUM complete (bebas apa pun modenya) ⭐ BUG FIX
    for (auto& polygon : polygonShapes) {
        if (!polygon.isAnimationComplete()) {
            polygon.update(deltaTime);
        }
    }
}
```

**Bug Fix - Animation Stopping Prematurely:**
- Sebelumnya: `updatePolygons()` hanya update saat `isLoadParallelMode() == true`
- Masalah: Setelah sequential load complete, `isLoadParallelMode` = false, animations stop
- Solusi: Selalu update polygons yang belum complete, regardless of mode

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
    float speed;       // 4 bytes - animation speed
    bool selected;     // 1 byte
    // Label & Duplicate fields
    int labelLength;   // 4 bytes
    char* label;       // variable length - "customLine0", "DcustomLine0", dst
    bool isDuplicate;  // 1 byte - True jika ini duplicate line (DcustomLine)
    AxisLock axisLock; // 4 bytes - NONE, LOCK_X, LOCK_Y, or LOCK_BOTH
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
    float speed;       // 4 bytes - animation speed
    bool selected;     // 1 byte
}
```

**Key Features:**
- **NORMALIZED Positions**: Semua posisi dibagi radius saat save, dikali radius saat load
- **Versioning**: Support untuk backward compatibility
- **Reserved Space**: 56 bytes untuk future expansion
- **Complete State**: Template, settings, lines, polygons, animation speeds semuanya tersimpan
- **Speed Sync**: Speed multiplier disimpan di SacredGeometry panel, sync ke FileManager saat load

**Load Process (via Tombol Draw di Playground):**

1. **Clean Canvas** - Hapus semua shapes, polygons, customLines, userDots + **reset settings** (L/P/D colors → blue, Dot → visible, Radius → 8.0f) ⭐ UPDATED
2. **Apply Settings** - Speed multiplier, polygon animation mode
3. **Load & Animate** - Baca file .nay dan animate sesuai mode yang dipilih:
   - **Parallel Per Group**: Template → CustomLines → Polygons animate simultaneously per group
   - **Sequential Per Group**: Groups animate satu per satu dengan delay

---

## 📁 Project Structure

```
BasicIslamicGeometry/
├── src/
│   ├── main.cpp              # Entry point aplikasi (1920x1080, OpenGL 4.6)
│   ├── ofApp.cpp/h           # Main application class (~3000+ lines)
│   ├── shape/                # Shape implementations
│   │   ├── AbstractShape.cpp/h         # Base class untuk semua shapes (no showing flag) ⭐ UPDATED
│   │   ├── CircleShape.cpp/h           # Circle dengan angle/distance positioning
│   │   ├── CartesianAxes.cpp/h         # X-Y axes dengan scaling animation
│   │   ├── CrossLine.cpp/h             # Diagonal lines dengan proportional scaling
│   │   ├── ParallelogramLine.cpp/h     # Connecting lines dengan intersections
│   │   ├── RectangleLine.cpp/h         # Rectangle dengan 2 intersection dots
│   │   ├── OctagramLine.cpp/h          # 2-phase animation lines
│   │   ├── CustomLine.cpp/h            # User-created bezier lines
│   │   ├── PolygonShape.cpp/h          # Fill-only polygons dengan animations
│   │   ├── DotShape.cpp/h              # Single dot shapes
│   │   └── DotInfo.h                   # Common struct untuk dot information
│   ├── anim/                 # Animation system
│   │   ├── AbstractAnimation.cpp/h    # Base class untuk animations
│   │   ├── FadeInAnimation.cpp/h      # Alpha fade effect dengan deltaTime
│   │   ├── WobbleAnimation.cpp/h      # Oscillation effect dengan deltaTime
│   │   └── FillAnimation.cpp/h        # Water fill effect dengan deltaTime
│   ├── template/             # Template system
│   │   ├── SacredGeometryTemplate.cpp/h  # Abstract template base
│   │   ├── TemplateRegistry.cpp/h        # Singleton registry
│   │   └── templates/
│   │       └── BasicZelligeTemplate.cpp/h # Moroccan pattern (26 shapes)
│   └── operation/            # Operations layer
│       ├── FileManager.cpp/h       # .nay save/load dengan speed sync
│       ├── FileOperationManager.cpp/h # File operations wrapper ⭐ NEW
│       └── gui/                    # ImGui components
│           ├── AbstractGuiComponent.cpp/h # GUI base
│           ├── MenuBar.cpp/h            # File/Edit/View menus
│           ├── SacredGeometry.cpp/h     # Template control panel (Transform UI) ⭐ UPDATED
│           ├── Playground.cpp/h         # Playback panel (Draw Settings UI) ⭐ UPDATED
│           ├── UserCustom.cpp/h         # User control panel
│           ├── ContextMenu.cpp/h        # Right-click context menu
│           ├── SuccessPopup.cpp/h       # Success dialog
│           ├── ErrorPopup.cpp/h         # Error dialog
│           ├── SelectionInfo.cpp/h      # Selected objects info window ⭐ NEW
│           └── ObjectTooltip.cpp/h      # Object tooltips manager ⭐ NEW
├── bin/                      # Compiled executable
│   └── data/                 # Saved workspaces (.nay files)
├── README.md                 # Comprehensive documentation (this file)
└── BasicIslamicGeometry.sln  # Visual Studio solution
```

**Total Files**: 70+ source files (.cpp + .h)

**Architecture Highlights:**
- **Template System**: SacredGeometryTemplate base class untuk extensibility
- **Template Registry**: Singleton pattern untuk managing patterns
- **GUI System**: Modular ImGui components dengan AbstractGuiComponent
- **Shape Hierarchy**: Semua shapes inherit dari AbstractShape (no show/hide)
- **Animation System**: AbstractAnimation base untuk reusable animations dengan deltaTime
- **Speed Control**: Centralized speed multiplier system untuk semua animations
- **GLSL Rendering**: Conditional GPU/CPU rendering untuk polygons berdasarkan `loadedFromFile` ⭐ NEW
- **FileOperationManager**: Wrapper pattern untuk file operations (mirip ColorManager) ⭐ NEW
- **Conditional UI**: CollapsingHeader hanya muncul jika file punya data ⭐ NEW
- **Skip Load**: CustomLines load bisa di-skip via checkbox ⭐ NEW
- **Object Tooltip System**: Custom OF rendering untuk selected objects info
- **UserDot System**: Flexible dot placement dengan radius dari slider
- **Color Management**: Smart sync antara objects dan color pickers
- **Undo/Redo**: 100-step history dengan comprehensive state tracking (termasuk CREATE_DOT radius)
- **File Operations**: FileOperationManager dengan direct file save dan speed sync ⭐ UPDATED
- **Window Management**: Independent window visibility controls
- **Transform System**: Canvas transform dengan inverse transform untuk mouse input

---

## 🎯 Purpose

Project ini adalah bagian dari eksplorasi **Creative Coding** dan pembelajaran:

- 🔬 Eksperimen geometric patterns dan Islamic art
- 🎨 Belajar animasi drawing dengan segment-based approach
- ⚡ Implementasi smooth animations dengan progress-based rendering dan deltaTime
- 📚 Memahami arsitektur modular untuk geometric visualizations
- 🌿 Fondasi untuk project Islamic geometric patterns yang lebih kompleks
- 💾 Workspace persistence untuk save/load creative work
- 🎛️ Professional GUI development dengan ImGui
- 🖼️ Canvas transform system untuk viewport control
- 📋 Selection info display untuk better UX
- 💡 Object tooltips untuk enhanced user experience
- ✨ UserDot system untuk flexible dot placement
- 🎭 GLSL shaders untuk advanced polygon rendering ⭐ NEW
- 🗂️ File operation manager pattern untuk clean architecture ⭐ NEW

---

## 🚀 Performance

Dengan optimasi C++ modern dan openFrameworks:

- **Solid 60 FPS** pada resolusi bervariasi (1920x1080 default)
- **Smooth drawing animation** tanpa lag dengan deltaTime system
- **Anti-aliased rendering** untuk kualitas visual tinggi
- **Hybrid Rendering System** - GLSL untuk loaded polygons, CPU untuk new polygons ⭐ NEW
- **Lazy caching** untuk dot position queries
- **Smart pointer optimization** untuk memory management
- **Consistent Animation Speed** - Delta time system memastikan speed konsisten
- **Efficient Rendering** - Draw Only concept menghemat resources
- **Update Strategy Optimization** - Hanya update incomplete animations ⭐ NEW

---

## 📝 Current Status: **sketch-islamic-gs-playgroundhandle**

Branch ini adalah **Islamic Geometry Studio** - aplikasi komprehensif untuk membuat, mengedit, dan menyimpan pola geometri Islam dengan GUI berbasis ImGui, sistem template yang modular, speed control global, transform canvas, draw only concept, object tooltips, userDot system, GLSL rendering, dan file operation manager.

### ✨ Key Features (Latest Updates)

✅ **FileOperationManager** - Wrapper class untuk semua file operations (save, open, load) ⭐ NEW
✅ **Conditional CollapsingHeader** - Custom Line/Polygon hanya muncul jika file punya data ⭐ NEW
✅ **Draw Custom Lines Checkbox** - Kontrol apakah customLines diload/digambar ⭐ NEW
✅ **Skip CustomLines Load** - Parallel dan sequential load skip customLines jika unchecked ⭐ NEW
✅ **GLSL Polygon Rendering** - Conditional rendering berdasarkan `loadedFromFile` flag ⭐ NEW
✅ **Polygon Animation Bug Fixes** - Fixed update logic untuk complete animations ⭐ NEW
✅ **Draw Order Bug Fix** - Fixed staggered load setup untuk proper draw order ⭐ NEW
✅ **Draw Only Concept** - Hanya shapes yang dicentang yang dibuat (tidak ada show/hide)
✅ **No More Showing Flag** - AbstractShape tidak punya `showing` flag
✅ **Forward Only Animation** - Shapes selalu forward animation (0 → maxProgress)
✅ **Delta Time Animation System** - Semua animasi menggunakan deltaTime untuk consistency
✅ **Global Speed Control** - Slider speed 0.1x - 1.5x berlaku untuk SEMUA animations
✅ **Transform Canvas System** - Pan X/Y, Rotate, Zoom controls dengan inverse transform
✅ **Selection Info Panel** - Floating window untuk selected objects info
✅ **Object Tooltip System** - Custom tooltips untuk selected objects (dots, lines, polygons)
✅ **UserDot System** - Duplicate dots dengan radius dari slider User Custom
✅ **Color Sync Improvements** - Better sync antara objects dan color pickers
✅ **Draw Template UI** - Parallel/Sequential draw di SacredGeometry panel
✅ **Clean & Draw Workflow** - Clean canvas dulu, baru draw dengan Draw button (Playground)
✅ **Auto Speed Sync** - Saat load file, speed mengikuti slider setting
✅ **UserCustom Panel** - Window untuk kontrol user (Dot, Line, Polygon)
✅ **Duplicate Line R 180°** - Duplicate selected lines dengan rotate 180° di global center
✅ **DcustomLine System** - Duplicate lines dengan isDuplicate flag dan axis lock system
✅ **Axis Lock System** - Control pergerakan DcustomLine (NONE, LOCK_X, LOCK_Y, LOCK_BOTH)
✅ **Context Menu System** - Per-line dan bulk operation untuk DcustomLine lock/unlock
✅ **Scroll Control** - Mouse scroll untuk menggerakkan DcustomLine sesuai axis lock
✅ **WobbleAnimation Fix** - Fixed polygons not wobbling dengan shader wobble.vert/frag ⭐ NEW

### 🧹 Clean Canvas Reset System ⭐ NEW
- **Complete Reset on Clean Canvas** - Saat Clean Canvas dipanggil (SacredGeometry / Edit menu):
  - **Color Pickers Reset** - L, P, D color pickers kembali ke warna biru default (0, 0, 255)
  - **Dot Visibility Reset** - Checkbox Dot di UserCustom menjadi checked (showUserDot = true)
  - **Radius Slider Reset** - Slider radius kembali ke default value (8.0f)
- **Consistent Default State** - Memastikan setiap kali canvas dibersihkan, settings kembali ke default
- **Better UX** - User tidak perlu manual reset settings setelah clean canvas

### 🐛 Bug Fixes (Latest Updates)

✅ **WobbleAnimation Bug** - Fixed polygons not wobbling/oscillating ⭐ NEW:
  - Problem: `PolygonShape::drawGLSL()` tidak ada rendering logic untuk WobbleAnimation
  - Result: Polygon tetap statis, tidak bergoyang meski Wobble dipilih
  - Fix: Tambahkan WobbleAnimation handling dengan shader wobble.vert/frag untuk vertex displacement

✅ **Wave Fill Animation Bug** - Fixed jagged edges after animation completes ⭐ NEW:
  - Problem: FillAnimation tidak dihapus setelah complete, shader wave tetap dipanggil
  - Result: Pinggiran polygon menjadi bergerigi (jagged edges) setelah wave fill selesai
  - Fix: Tambahkan logic untuk hapus FillAnimation dan switch ke No Animation setelah complete (sama seperti WobbleFillAnimation/GradientAnimation)

✅ **Polygon Animation Bug** - Fixed polygons stopping mid-animation ⭐ NEW:
  - Problem: `updatePolygons()` hanya update saat `isLoadParallelMode() == true`
  - Result: Setelah sequential load complete, animations stop prematur
  - Fix: Selalu update polygons yang belum complete, regardless of mode

✅ **Draw Order Bug** - Fixed polygons appearing before template shapes ⭐ NEW:
  - Problem: Sequential Per Group mode showed polygons first
  - Result: Wrong visual order (should be: template → customLines → polygons)
  - Fix: Added staggered load setup to `loadWorkspace()` (missing initialization)

✅ **FadeIn Animation Bug** - Fixed last polygon not colored correctly ⭐ NEW:
  - Problem: Update logic stopping before animation complete
  - Result: Last polygon tidak terwarnai dengan benar
  - Fix: Check `isAnimationComplete()` before updating

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
- **[Watch Demo](https://youtu.be/A50uG7nty7U)** - YouTube demonstration
- **[SandyKurt Tutorials](https://sandykurt.com/free-tutorials)** - Free Islamic geometric patterns tutorials
- **[Support Me](https://sociabuzz.com/abdkdhni)** - Fund the experiments ☕

---

**Made with ❤️ for learning and creative exploration**
