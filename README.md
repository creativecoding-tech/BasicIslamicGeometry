## BasicIslamicGeometry | OpenFrameworks

Eksperimen geometri Islam dengan pola lingkaran yang saling berhubungan dan animasi drawing yang smooth. Project ini adalah eksplorasi creative coding dengan openFrameworks untuk membuat pola geometri Islam yang fundamental dengan sistem animasi yang halus.

[![OpenFrameworks](https://img.shields.io/badge/OpenFrameworks-0.12.1-blue)](https://openframeworks.cc/)
![C++](https://img.shields.io/badge/C++-17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![License](https://img.shields.io/badge/License-Apache%202.0-green)
![Branch](https://img.shields.io/badge/Branch-sketch-islamic-gs-tesselation-orange)

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
  - `peekFilePolygonCount()` - Baca jumlah polygons dari file (hanya poligon original) ⭐ UPDATED
- **Conditional UI Display** - CollapsingHeader hanya muncul jika file punya data:
  - Custom Line CollapsingHeader hanya muncul jika `loadedFileCustomLinesCount > 0`
  - Polygon CollapsingHeader hanya muncul jika `loadedFilePolygonCount > 0`

### Polygon Seamless Tessellation ⭐
- **Pattern Infusion** - Mengisi area poligon dengan pola geometri dari file `.nay` lain secara otomatis.
- **Mathematical Tiling** - Sistem tiling yang presisi untuk memastikan antar tile tidak ada celah (*seamless*) dan tidak tumpang tindih (*no overlaps*).
- **Radius-Based Scaling** - Besar kecilnya pola di dalam poligon dikontrol melalui slider Radius di tabel Tessellation.
- **Geometric Clipping** - Hanya bentuk yang titik pusatnya berada di dalam poligon target yang akan di-render.
- **Filtered UI** - Poligon hasil teselasi secara otomatis disembunyikan dari daftar pengaturan untuk mencegah kebingungan dan pengulangan teselasi (*no recursive tessellation*).
- **Batch Rendering Optimization** - Tessellated polygons dirender dalam **SATU draw call** menggunakan ofMesh batch:
  - ~4,500 tessellated polygons dengan **2.4+ juta vertices** dirender sekaligus
  - Triangle fan triangulation untuk optimal mesh construction
  - Dirty flag system untuk rebuild mesh hanya saat polygons berubah
  - Solid 60 FPS performance bahkan dengan ribuan tessellated polygons ⭐ NEW
- **Source Tessellation Metadata** - Setiap parent polygon menyimpan metadata tessellation (file + radius) untuk prevent re-tessellation
- **Smart Tessellation System** - Otomatis detect perubahan tessellation settings:
  - Cek apakah UI settings (file, radius) match dengan loaded state
  - Jika berubah, otomatis hapus old children dan tessellate dengan setting baru
  - Jika sama dan children sudah ada, skip tessellation (performance optimization)
  - Tombol Browse/X untuk ganti/clear tessellation file per polygon ⭐ NEW

### Intelligent Save System ⭐ NEW
- **Save Confirmation Popup** - Muncul otomatis jika mendeteksi ada geometri hasil teselasi saat menyimpan workspace (CTRL+S / CTRL+SHIFT+S).
- **Flexible Options** - User bisa memilih untuk menyimpan hanya geometri asli (*Original Only*) atau menyertakan hasil teselasinya (*With Tessellation*).
- **Version 3 .nay Format** - Mendukung flag `isTessellated` secara persisten untuk membedakan geometri asli dengan geometri hasil regenerasi otomatis.

### Canvas Settings System ⭐ NEW
- **Canvas Settings Window** - Floating window untuk mengatur tampilan canvas:
  - **Base Color Picker** - Warna background dasar (RGBA color edit)
  - **Use Gradient Checkbox** - Enable/disable gradient background
  - **Gradient Color Picker** - Warna gradient (hanya muncul jika gradient dicentang)
  - **Trails Mode** - Toggle trails effect (0: No Trails, 1: Use Trails)
  - **Trails Value Slider** - Kontrol opacity trails (1-255, default 25)
  - **Reset Button** - Reset semua settings ke default (white background, no gradient, trails enabled)
- **Gradient Background** - Vertical gradient dari Base Color (top) ke Gradient Color (bottom)
- **Trails Effect** - Semi-transparent background overlay untuk efek jejak visual:
  - Trails opacity 1-255 (semakin kecil = semakin panjang jejak)
  - Direct rendering TANPA Frame Buffer Object (FBO) untuk performance ⭐ NEW
  - Automatic background drawing setiap frame dengan alpha blending
- **View Menu Integration** - Canvas Settings bisa diakses via View → Canvas Settings
- **Force Clear Screen System** - Saat clean canvas atau Draw, otomatis:
  - Pause animation selama force clear (prevent trails contamination)
  - Clear screen dengan solid background (20 frame / ~0.33 detik)
  - Resume animation dengan normal trails (20 frame / ~0.33 detik)
  - Total delay ~40 frame (0.67 detik) untuk menghilangkan jejak ImGui window ⭐ NEW
- **Canvas Transform Not Saved** - Canvas settings dan transform state TIDAK disimpan ke file .nay (viewport-only)

### Tessellation Button & Settings ⭐
- **Tessellation Button** - Tombol di Playground window (lebar 140px, di atas Draw button)
  - Membuka popup Tessellation Settings di sebelah kiri Playground
  - Popup posisi dinamis menyesuaikan ukuran konten
  - Auto adjust posisi Y saat Mode muncul/dihilangkan
- **Tessellation Settings Popup** - Floating panel dengan kontrol tessellation:
  - **Draw Radio Button** - Toggle tessellation On/Off (No/Yes)
    - No: Tessellation dinonaktifkan (default)
    - Yes: Tessellation diaktifkan, Mode options muncul
  - **Mode Selection** - Pilihan metode tessellation (hanya muncul jika Yes):
    - **Post-Draw** ✅ IMPLEMENTED - Gambar bentuk normal dulu dengan radius dari file .nay, animasi selesai, clean canvas, lalu gambar lagi dengan tessellation radius
    - **Direct** 🚧 WIP - Langsung tessellasi di posisi, gambar & animasi di posisi tessellation
  - **Pre-Tessellation Slider** - Durasi pause sebelum tessellation dimulai (0-5 detik, default 0)
    - Hanya muncul jika Mode = Post-Draw atau Direct
    - Memberikan jeda sebelum clean canvas dan redraw tessellation
  - **Radius DragFloat** - Kontrol radius tessellation (25-214, default 120)
    - Menentukan ukuran pola tessellation
    - SacredGeometry slider menampilkan tessellationRadius saat tessellation aktif
- **Post-Draw Mode Behavior** (IMPLEMENTED):
  - **Step 1**: Gambar normal dengan radius dari file .nay
  - **Step 2**: Tunggu hingga animasi selesai
  - **Step 3**: Pre-Tessellation pause (jika di-set > 0)
  - **Step 4**: Clean canvas (tanpa reset speed)
  - **Step 5**: Gambar ulang dengan tessellation radius
  - **Draw Mode Preservation**: Tessellation menggunakan mode yang sama dengan draw pertama (Sequential/Parallel)
  - **Template Speed Preservation**: Tessellation menghormati Template Speed slider dari Playground ✅ FIXED
  - **Draw Settings Preservation**: Tessellation menghormati checkbox Draw Settings dari Playground ✅ FIXED
  - **SacredGeometry Slider**: Tetap di tessellationRadius setelah tessellation selesai (tidak restore ke radius asli)
- **Dynamic Popup Behavior** - Popup secara otomatis:
  - Mengubah posisi Y berdasarkan state Yes/No
  - Menyesuaikan ukuran berdasarkan konten yang aktif
  - Tetap di sebelah kiri Playground window dengan proper spacing

### Tessellation Canvas (Square Grid Tiling) 🚧 WIP
- **What is Tessellation Canvas?**
  - Men-copy pattern geometri (Template + Custom Lines + Polygons) ke grid di seluruh canvas
  - Square grid tiling untuk menutup canvas tanpa celah (seamless coverage)
  - Setiap tile berisi complete pattern geometry

- **Difference from Polygon Tessellation:**
  - **Polygon Tessellation** (sudah ada): Pattern infusion ke dalam polygon area (clipping)
  - **Tessellation Canvas** (yang ini): Pattern replication ke grid positions di seluruh viewport

- **Square Grid System:**
  - **Tile Size**: tessellationRadius × 2
  - **Grid Layout**: Square grid (tidak brick/hexagonal pattern)
  - **Coverage**: Seluruh viewport + margin 1 tile di setiap sisi
  - **Origin**: Top-left viewport (-tileSize, -tileSize)
  - **No Culling**: Semua tiles di-draw (termasuk yang di luar viewport)

- **Tessellation Scope (ALL Geometry):**
  - ✅ Template shapes (Cartesian, Circles, CrossLines, Parallelograms, RectangleLines, OctagramLines)
  - ✅ Custom Lines (jika ada di file .nay)
  - ✅ Polygons (jika ada di file .nay)
  - Note: Jika file .nay hanya ada template, tessellation hanya untuk template

- **Animation Modes:**

  **Parallel Mode** (Recommended & Simpler):

  **Parallel Tessellation Settings** (muncul di popup ketika Playground = Parallel):
  - Group: **"Template Parallel"** (di bawah Radius)
  - Pilihan:
    1. **Synchronous** - Semua tile mulai animasi bersamaan
    2. **Radial Expansion** - Mulai dari tengah grid, menyebar keluar

  **Option 1: Synchronous** ⚡ (Simple & Fast)
  ```
  Frame 0:      SEMUA tile mulai BERSAMAAN
                Tile(0,0): Shape 1,2,3... paralel
                Tile(0,1): Shape 1,2,3... paralel
                Tile(0,2): Shape 1,2,3... paralel
                Tile(1,0): Shape 1,2,3... paralel
                Tile(1,1): Shape 1,2,3... paralel
                ... (SEMUA tiles synchronous)

  Frame 100:    Seluruh canvas tessellation visible!
  ```
  - **Behavior**: Semua tile di grid mulai animasi di frame yang sama (t = 0)
  - **Implementation**: Shared template, multiple draw calls dengan offset berbeda
  - **Visual Effect**: Pattern muncul serentak di seluruh canvas seperti "instant reveal"
  - **Performance**: Most efficient (1 template instance, N transforms)
  - **Duration**: shapeCount × durationPerShape (sama dengan single tile)

  **Option 2: Radial Expansion** 🌊 (Artistic & Dynamic)
  ```
  Frame 0:      Tile di CENTER grid mulai dulu
                Tile(centerRow, centerCol): Shape 1,2,3... paralel

  Frame 50:     Tiles di RING 1 (distance = 1 dari center) mulai
                Tile(centerRow-1, centerCol-1): Shape 1,2,3... paralel
                Tile(centerRow-1, centerCol): Shape 1,2,3... paralel
                Tile(centerRow-1, centerCol+1): Shape 1,2,3... paralel
                Tile(centerRow, centerCol-1): Shape 1,2,3... paralel
                Tile(centerRow, centerCol): Already complete
                Tile(centerRow, centerCol+1): Shape 1,2,3... paralel
                Tile(centerRow+1, centerCol-1): Shape 1,2,3... paralel
                Tile(centerRow+1, centerCol): Shape 1,2,3... paralel
                Tile(centerRow+1, centerCol+1): Shape 1,2,3... paralel

  Frame 100:    Tiles di RING 2 (distance = 2 dari center) mulai
                 ...

  Visual: Pattern "ripple" menyebar dari tengah ke pinggir seperti batu jatuh ke air
  ```
  - **Behavior**: Tiles mulai animasi berdasarkan jarak dari center grid (Manhattan distance atau Euclidean distance)
  - **Ring System**: Tiles dikelompokkan berdasarkan distance dari center
    - Ring 0: Center tile (distance = 0)
    - Ring 1: 8 tiles di sekeliling center (distance = 1)
    - Ring 2: 16 tiles di ring luar (distance = 2)
    - ...dst
  - **Implementation**: Per-ring delay system, tiles dalam ring yang sama mulai bersamaan
  - **Visual Effect**: "Ripple" atau "shockwave" dari tengah ke pinggir, sangat artistic
  - **Performance**: Medium complexity (ring-based grouping)
  - **Duration**: (maxRingCount × shapeCount) × durationPerShape

  **Distance Calculation Options:**
  - **Manhattan Distance**: |row - centerRow| + |col - centerCol| (diamond pattern)
  - **Euclidean Distance**: √((row - centerRow)² + (col - centerCol)²) (circular pattern)
  - **Chebyshev Distance**: max(|row - centerRow|, |col - centerCol|) (square pattern) ⭐ RECOMMENDED for square grid

  **Tessellation Scope by Geometry Type:**

  **1. Template Tessellation** (Priority 1 - Handle First)
  - Mode: **Synchronous** atau **Radial Expansion**
  - Settings: Di popup Tessellation Settings, group "Template Parallel"
  - Animasi: Menghormati Template Speed slider dari Playground
  - Draw Settings: Menghormati checkbox (Cartesian, Circles, CrossLines, dll)

  **2. Custom Lines Tessellation** (Priority 2 - Handle After Template)
  - Mode: **Synchronous** atau **Radial Expansion** (pilihan terpisah dari template)
  - Settings: Di popup Tessellation Settings, group "Custom Lines Parallel" (di bawah Template Parallel)
  - Only if: loadedFileCustomLinesCount > 0 (ada custom lines di file .nay)
  - Wave Animation: Menghormati Wave settings dari Playground (jika enabled)

  **3. Polygons Tessellation** (Priority 3 - Handle Last)
  - Mode: **Synchronous** atau **Radial Expansion** (pilihan terpisah)
  - Settings: Di popup Tessellation Settings, group "Polygons Parallel" (di bawah Custom Lines Parallel)
  - Only if: loadedFilePolygonCount > 0 (ada polygons di file .nay)
  - Animation: Menghormati Polygon Animation settings dari Playground (Fade In, Wobble, dll)
  - Note: Polygons hasil tessellation polygon (isTessellated=true) TIDAK di-tessellate lagi

  **Implementation Approach:**
  - **Phase 1**: Template tessellation (Synchronous + Radial Expansion)
  - **Phase 2**: Custom lines tessellation (Synchronous + Radial Expansion)
  - **Phase 3**: Polygons tessellation (Synchronous + Radial Expansion)
  - Each phase independent, bisa dipilih on/off per geometry type

### Tessellation Canvas Implementation Status ✅

**Phase 1: Template Tessellation (Parallel - Synchronous)** ✅ **COMPLETED** (2026-03-05)

✅ **Implemented & Tested:**
- TessellationManager class created (square grid calculation)
- TessellationManager registered to .vcxproj and .vcxproj.filters
- TessellationManager initialized in ofApp
- Inverse canvas transform calculation untuk accurate grid positioning
- Generate tessellation grid in LOAD_DONE (Post-Draw mode)
- Tessellation grid cleared on clean canvas
- Direct rendering system (NO FBO cache untuk smooth anti-aliased lines)
- Grid generation TANPA margin untuk cegah double lines di tile boundaries
- Draw logic for Synchronous mode (all tiles animate together)
- UI: "Template Parallel" group in Tessellation Settings popup
  - Synchronous radio button (default) ✅
  - Radial Expansion radio button (UI ready, implementation TBD)
  - Only appears when Playground = Parallel mode
- TessellationTemplateParallelMode saved to app state
- Popup Y-offset adjustment for Parallel mode

✅ **Testing Results:**
- ✅ Performance: Solid 60 FPS dengan 165 tiles × 26 shapes
- ✅ Quality: Smooth anti-aliased lines (direct OpenGL rendering with MSAA)
- ✅ Positioning: Grid covers entire viewport dengan benar
- ✅ Seamless: Tidak ada double lines di tile boundaries
- ✅ Animation: Synchronous mode berfungsi - semua tiles animate bersamaan

🚧 **Phase 2: Custom Lines Tessellation** - NOT STARTED
🚧 **Phase 3: Polygons Tessellation** - NOT STARTED
🚧 **Phase 4: Radial Expansion Mode** - NOT STARTED

  **Sequential Mode** (Complex - TBD):

  **Option A: Sequential per Row** 🌊 Wave Pattern
  ```
  Frame 0-100:   Row 0 semua tile animasi sequential
                 Tile(0,0): Shape 1→2→3→... (sequential)
                 Tile(0,1): Shape 1→2→3→... (sequential)
                 Tile(0,2): Shape 1→2→3→... (sequential)
                 (Row 0 complete)

  Frame 100-200: Row 1 semua tile animasi sequential
                 Tile(1,0): Shape 1→2→3→... (sequential)
                 Tile(1,1): Shape 1→2→3→... (sequential)
                 Tile(1,2): Shape 1→2→3→... (sequential)
                 (Row 1 complete)

  Frame 200-300: Row 2 semua tile animasi sequential
                 ...

  Visual: Pattern "waterfall" dari atas ke bawah
  ```
  - **Kelebihan**: Visual menarik seperti wave untaian tasbih
  - **Kekurangan**: Total duration = (rows × shapeCount) × durationPerShape
  - **Implementation**: Per-row state management, complexity medium

  **Option B: All Tiles Sequential Together** 🔄 Synchronous Pattern
  ```
  Frame 0-50:    SEMUA tile animasi Shape 1 bersamaan
                 Tile(0,0): Shape 1 complete
                 Tile(0,1): Shape 1 complete
                 Tile(0,2): Shape 1 complete
                 Tile(1,0): Shape 1 complete
                 ... (semua tile Shape 1 complete)

  Frame 50-100:  SEMUA tile animasi Shape 2 bersamaan
                 Tile(0,0): Shape 2 complete
                 Tile(0,1): Shape 2 complete
                 Tile(0,2): Shape 2 complete
                 Tile(1,0): Shape 2 complete
                 ... (semua tile Shape 2 complete)

  Frame 100-150: SEMUA tile animasi Shape 3 bersamaan
                 ...

  Visual: Seluruh canvas "berkedip" shape-by-shape synchronous
  ```
  - **Kelebihan**: Total duration = shapeCount × durationPerShape (sama dengan single tile!)
  - **Kekurangan**: Tidak ada "flow" antar tile, kurang dinamis
  - **Implementation**: Shared template state, simplest sequential option

  **Option C: True Sequential (One by One)** 📜 Snake Pattern
  ```
  Frame 0-50:    Tile(0,0) complete (Shape 1→2→3→...)

  Frame 50-100:  Tile(0,1) complete (Shape 1→2→3→...)

  Frame 100-150: Tile(0,2) complete (Shape 1→2→3→...)

  Frame 150-200: Tile(1,0) complete (Shape 1→2→3→...)
                 ... (bergantian satu per satu)

  Visual: Pattern "ular" atau kursor berjalan melintasi grid
  ```
  - **Kelebihan**: Sangat dramatic, mata user follow tile progression
  - **Kekurangan**: Total duration = (totalTiles × shapeCount) × durationPerShape (sangat lama!)
  - **Implementation**: Per-tile state management, highest complexity

  **Option D: Snake/Zigzag per Row** 🐍 Enhanced Wave Pattern
  ```
  Frame 0-100:   Row 0 LEFT→RIGHT
                 Tile(0,0) → Tile(0,1) → Tile(0,2) → ... (sequential)

  Frame 100-200: Row 1 RIGHT→LEFT (zigzag!)
                 Tile(1,5) → Tile(1,4) → Tile(1,3) → ... (sequential reverse)

  Frame 200-300: Row 2 LEFT→RIGHT
                 Tile(2,0) → Tile(2,1) → Tile(2,2) → ... (sequential)

  Visual: Pattern "snaking" seperti mengisi formir zigzag
  ```
  - **Kelebihan**: Lebih dinamis dari Option A, continuous flow
  - **Kekurangan**: Total duration = (rows × cols) × shapeCount × durationPerShape
  - **Implementation**: Direction-aware row state, complexity high

  **Recommendation for Islamic Geometry:**
  - **Option B** (All Tiles Sequential Together) untuk **initial implementation**
    - Simple, predictable duration
    - Masuskan dengan symmetric nature Islamic pattern
    - Efficient implementation
  - **Option A** atau **Option D** untuk **future enhancement**
    - Lebih artistic dan visually pleasing
    - Cocok untuk showcase/presentation

  **To Be Determined**: User akan memilih pattern yang sesuai vision

- **Implementation Status:**
  - ✅ TessellationManager class created
  - ✅ Square grid calculation (generateGrid)
  - 🚧 Draw logic untuk tessellation instances (in progress)
  - 🚧 Custom lines tessellation (pending)
  - 🚧 Polygons tessellation (pending)
  - 🚧 Sequential mode pattern selection (pending)

### Transform Canvas System
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
  - **View Menu**: Sacred Geometry, Playground, User Custom, Canvas Settings, Selection Info (show/focus windows independently)
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
  - **OrbitLeftAnimation** - Animasi orbit berlawanan arah jarum jam ⭐ NEW
    - Polygon mengorbit/mengelilingi titik (0,0) / Circle A (counter-clockwise)
    - Half-cosine wave motion untuk osilasi mulus: 0° → -maxAngle → 0°
    - Amplitudo orbit: 90 derajat (default, dapat diubah via UI slider: 35° - 360°)
    - Finite animation: berhenti setelah 1 siklus penuh (±2 detik)
    - getAngle() - Sudut orbit saat ini dalam derajat
  - **OrbitRightAnimation** - Animasi orbit searah jarum jam ⭐ NEW
    - Polygon mengorbit/mengelilingi titik (0,0) / Circle A (clockwise)
    - Half-cosine wave motion untuk osilasi mulus: 0° → +maxAngle → 0°
    - Amplitudo orbit: 90 derajat (default, dapat diubah via UI slider: 35° - 360°)
    - Finite animation: berhenti setelah 1 siklus penuh (±2 detik)
    - getAngle() - Sudut orbit saat ini dalam derajat
  - **SpinLeftAnimation** - Animasi spin berlawanan arah jarum jam ⭐ NEW
    - Polygon berputar pada porosnya sendiri/centroid (counter-clockwise)
    - Half-cosine wave motion untuk osilasi mulus: 0° → -maxAngle → 0°
    - Amplitudo spin: 90 derajat (default, dapat diubah via UI slider: 35° - 360°)
    - Finite animation: berhenti setelah 1 siklus penuh (±2 detik)
    - getAngle() - Sudut spin saat ini dalam derajat
  - **SpinRightAnimation** - Animasi spin searah jarum jam ⭐ NEW
    - Polygon berputar pada porosnya sendiri/centroid (clockwise)
    - Half-cosine wave motion untuk osilasi mulus: 0° → +maxAngle → 0°
    - Amplitudo spin: 90 derajat (default, dapat diubah via UI slider: 35° - 360°)
    - Finite animation: berhenti setelah 1 siklus penuh (±2 detik)
    - getAngle() - Sudut spin saat ini dalam derajat
  - **Configurable Speed** - Animation speed dapat di-adjust via speed slider
  - **Special Polygon Animation** - System untuk dual-animation pada polygon ⭐ NEW
    - Polygon dapat memiliki 2 animations sekaligus:
      - **Appearance Animation** - FadeIn, Wobble, Fill, Gradient, atau None
      - **Special Animation** - Orbit Left/Right atau Spin Left/Right (berlapis di atas appearance animation)
    - Independent control - Masing-masing animation berjalan independently
    - Special animation di-update HANYA setelah appearance animation complete
    - UI Control - Radio buttons di SacredGeometry window untuk set special animation per polygon
    - Angle Control - DragFloat slider RT (35° - 360°) untuk mengatur amplitudo orbit/spin
    - Default: Semua polygons default "No Animation" untuk special animation
  - **Perbedaan Orbit vs Spin**:
    - **Orbit** = Polygon mengelilingi titik (0,0) / Circle A (seperti Bumi mengelilingi Matahari)
    - **Spin** = Polygon berputar pada porosnya sendiri/centroid (seperti Bumi berotasi pada sumbunya)
  - **Configurable Speed** - Animation speed dapat di-adjust via speed slider

### Special Polygon Animation System ⭐ LATEST
- **5 Mode Special Animation** - Setiap polygon dapat memiliki special animation independen:
  - **No Animation (0)** - Polygon statis setelah appearance animation selesai
  - **Orbit Left (1)** - Polygon mengelilingi (0,0) counter-clockwise (0° → -maxAngle → 0°)
  - **Orbit Right (2)** - Polygon mengelilingi (0,0) clockwise (0° → +maxAngle → 0°)
  - **Spin Left (3)** - Polygon berputar pada porosnya sendiri counter-clockwise (0° → -maxAngle → 0°)
  - **Spin Right (4)** - Polygon berputar pada porosnya sendiri clockwise (0° → +maxAngle → 0°)
- **Angle Control** - DragFloat slider RT untuk mengatur amplitudo orbit/spin (35° - 360°)
- **Pause Duration Control** ⭐ NEW - Slider Pause (0.0 - 1.0 detik) untuk Orbit Left/Right dan Spin Left/Right:
  - Polygon akan pause di posisi maxAngle selama durasi yang ditentukan
  - Setelah pause selesai, polygon kembali ke posisi awal (0°)
  - Default: 0.0 detik (tidak ada pause)
  - Hanya muncul ketika Orbit atau Spin animation dipilih
- **Independent Speed Control** - Special Speed slider mengontrol kecepatan orbit/spin secara terpisah dari appearance speed
- **UI Table Layout** - 3 kolom: Polygon Name, Animation Type (radio buttons), Angle Slider + Pause Slider
- **Per-Polygon Configuration** - Setiap polygon dapat dikonfigurasi secara individual
- **Animation Timing** - Special animation berjalan HANYA setelah appearance animation complete
- **Dual Animation Stack** - Appearance animation + Special animation dapat berjalan simultaneously (setelah appearance complete)
- **Finite Duration** - Semua orbit/spin animations berhenti setelah 1 siklus penuh (±2 detik)
- **Orbit Implementation** - Menggunakan `ofRotateDeg()` untuk global rotation mengelilingi (0,0)
- **Spin Implementation** - Menggunakan transform matrix: Translate → Rotate → Translate back ke centroid

### Scaling System ⭐ NEW
- **Floating Point Drift Prevention** - System untuk mencegah presisi hilang saat radius slider diubah berulang kali
- **Original Vertices Backup** - Setiap shape (CustomLine, PolygonShape, DotShape) menyimpan titik asli:
  - `saveOriginalPoints(radius)` - Simpan titik asli CustomLine + baseRadius
  - `saveOriginalVertices(radius)` - Simpan vertices asli PolygonShape + baseRadius
  - `saveOriginalPosition(radius)` - Simpan posisi asli DotShape + baseRadius
- **Absolute Ratio Scaling** - Scaling menggunakan ratio absolute dari original, bukan relative:
  - Old way: `newPosition = currentPosition * (newRadius / oldRadius)` - akumulasi error
  - New way: `newPosition = originalPosition * (newRadius / baseRadius)` - presisi terjaga
- **Apply on Load** - Saat load file .nay, original vertices/points/position otomatis di-backup
- **Consistent Precision** - Polygon/titik tetap presisi walaupun radius slider diubah berkali-kali

### Custom Lines & Polygons
- **Mouse Interaction** - Mouse drag untuk menggambar line secara interaktif antar dots
- **CustomLine System** - Garis custom yang bisa dibuat user dengan mouse drag (start dot → end dot)
- **Bezier Curve Support** - Garis bisa dibuat melengkung dengan scroll mouse (curve parameter)
- **Multi-Select System** - CTRL+Klik untuk toggle selection multiple garis
- **CustomLine Labels** - Saat selected, muncul label dari getLabel() (customLine0, DcustomLine0, dst)
- **Curve Label** - Display nilai curve saat garis di-select
- **Draw Custom Lines Checkbox** - Checkbox "Draw Custom Lines" mengontrol apakah customLines digambar saat Draw:
  - ✓ Dicentang = CustomLines diload dan digambar
  - ✗ Tidak dicentang = CustomLines TIDAK diload (di-skip saat load)
- **Skip CustomLines Load** - Parallel vs Sequential load behavior:
  - **Parallel load**: CustomLines diload, lalu di-clear jika flag false
  - **Sequential load**: CustomLines TIDAK ditambahkan ke buffer jika flag false

### CustomLine Animation Controls ⭐ NEW
- **Line Animation Mode** - Control animasi untuk custom lines yang diload dari file .nay:
  - **No Animation** - Custom lines digambar tanpa animasi wave (progressive drawing saja)
  - **Wave Animation** - Custom lines digambar dengan efek wave/gelombang yang halus
- **Wave Animation Settings** - Parameter untuk wave effect (hanya muncul jika Wave Animation dipilih):
  - **Amplitude (px)** - Tinggi gelombang dalam pixels (2.0 - 5.0 px)
  - **Frequency** - Kekerapan gelombang (1.0 - 3.0 Hz)
  - **Duration (sec)** - Durasi wave animation sebelum auto-stop (0.0 - 60.0 detik)
    - 0.0 detik = Infinite loop (wave tidak berhenti otomatis)
    - > 0.0 detik = Wave berhenti setelah durasi habis
- **Step Animation Line** - Mengatur TIMING kapan wave animation dijalankan terhadap polygon animation:
  - **Before Polygon Draw** - Wave animation BERJALAN dan SELESAI sebelum polygons digambar:
    1. Custom lines selesai progressive drawing (0% → 100%)
    2. Wave animation diaplikasikan ke semua custom lines
    3. Wave animation berjalan selama durasi yang ditentukan
    4. Setelah durasi habis, wave animation dihapus
    5. Polygons baru mulai digambar dan beranimasi
  - **With Polygon Draw** - Wave animation dan polygon animation BERJALAN BARENGAN:
    1. Custom lines selesai progressive drawing
    2. Wave animation diaplikasikan ke semua custom lines
    3. Polygons mulai digambar dan beranimasi
    4. Wave animation terus berjalan selama polygons beranimasi
    5. Kedua animasi selesai secara independent
  - **After Polygon Draw** - Wave animation dijalankan SETELAH polygon animation selesai:
    1. Custom lines selesai progressive drawing
    2. Polygons mulai digambar dan beranimasi
    3. Setelah SEMUA polygons selesai beranimasi
    4. Wave animation diaplikasikan ke semua custom lines
    5. Wave animation berjalan selama durasi yang ditentukan
- **Staggered Load Integration** - Step Animation Line bekerja dengan staggered load system (Parallel Per Group mode):
  - **LOAD_TEMPLATE stage**: Template shapes digambar terlebih dahulu
  - **LOAD_CUSTOMLINES stage**: Custom lines diload dan di-animasikan sesuai mode
    - **With Polygon Draw**: Animasi dimulai SETELAH semua lines selesai digambar (parallel with polygons). ⭐ NEW
    - **After Polygon Draw**: Animasi dipaksa berhenti selama loading phase (nunggu polygons selesai). ⭐ NEW
  - **LOAD_POLYGONS stage**: Polygons diload dan di-animasikan sesuai mode
  - **LOAD_DONE stage**: Semua animasi selesai, wave animation dihapus (jika ada durasi)
- **Conditional UI Visibility** ⭐ NEW - Step Animation Line radio buttons (Before/With/After) hanya muncul jika file `.nay` mengandung polygons. Jika tidak ada polygons, sistem otomatis menggunakan mode `Before Polygon Draw` untuk memastikan animasi wave tetap berjalan.
- **Playground Window Behavior** ⭐ NEW - Peningkatan pada close confirmation popup:
  - **Yes, Close File**: Menutup window dan mengakhiri session file (lastSavedPath di-clear).
  - **No, Keep File Open**: Menutup window tetapi tetap menjaga session file tetap aktif demi kenyamanan user.
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
  - **Duplicate Dot Track** ⭐ NEW - Buat dot yang bisa bergerak沿着 customLine/DcustomLine:
    - Dot dibuat di midpoint customLine/DcustomLine
    - Link ke line via `trackLineIndex` untuk movement
    - Mouse scroll untuk menggerakkan dot sepanjang line (0.0 - 1.0 progress)
    - Mendukung bezier curves (bukan hanya garis lurus)
    - **Boundary margin** - Dot tidak bisa mencapai ujung line (5% margin di setiap ujung) ⭐ NEW
    - Connected lines otomatis update points ketika dot bergerak
- **Context Menu Access** - Right-click pada original dot untuk show duplicate options
- **Radius from Slider** - New userDot radius diambil langsung dari slider User Custom (0-8) ⭐ UPDATED
  - Tidak lagi dipengaruhi lineWidth template
  - Consistent dengan user expectations
- **Color from Picker** - New userDot color diambil dari color picker User Custom
- **Lower Bound System** - Setiap userDot punya lower bound (dot parent position) untuk reference
- **Scroll Control** - Mouse scroll untuk menggerakkan selected userDots:
  - **Track dots** (highest priority) - Dots yang ter-link ke customLine/DcustomLine bergerak sepanjang line path
  - Horizontal dots (left/right) → Scroll gerakkan di sumbu X
  - Vertical dots (above/below) → Scroll gerakkan di sumbu Y
  - Dengan boundary validation (tidak bisa melewati dot parent / ujung line)
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
- **.nay Binary Format** - Workspace format dengan magic number "NA01", version 3 ⭐ UPDATED
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
- **Batch Tessellated Mesh** - 4,500+ tessellated polygons (2.4M vertices) dirender dalam **1 draw call** ⭐ NEW
- **Triangle Fan Triangulation** - Optimal mesh construction untuk tessellated polygons
- **Dirty Flag System** - Batch mesh hanya rebuild saat polygons berubah (tidak setiap frame)
- **Anti-Aliasing & Smoothing** - Garis yang smooth untuk visual yang lebih baik
- **60 FPS Performance** - Solid 60 FPS bahkan dengan ribuan tessellated polygons
- **No Frame Buffer Object (FBO)** - Direct rendering dengan trails effect tanpa FBO overhead ⭐ NEW
- **OpenGL State Management** - Proper alpha blending dan color management untuk batch rendering ⭐ NEW

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
- **Smart Pointer Management** - Menggunakan `std::unique_ptr` dan `std::shared_ptr` secara eksplisit untuk resource management otomatis (RAII).
- **No Memory Leaks** - Audit memori menunjukkan penggunaan RAII yang solid, virtual destructors pada base classes (AbstractAnimation), dan penanganan OpenGL resources yang aman di PolygonShape (mencegah VRAM leaks). ⭐ UPDATED
- **Modular Design** - Terpisah dalam kategori: `shape/`, `anim/`, `operation/`, `template/`, `operation/gui/`
- **Memory-safe Implementation** - Modern C++17 features dengan RAII, smart pointers, move semantics

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
|  - **Custom Line Appearance** | CustomLine controls (color, curve, etc) ⭐ UPDATED: |
|  - **Animation Mode** | Pilihan animasi untuk custom lines: |
|  - **No Animation Line** | CustomLines digambar tanpa animasi wave |
|  - **Wave Animation Line** | CustomLines digambar dengan efek wave/gelombang ⭐ NEW |
|  - **Wave Settings** (Wave Animation only) | Slider untuk parameter wave: |
|  - **Amplitude (px)** | Tinggi gelombang dalam pixels (2.0 - 5.0 px) ⭐ NEW |
|  - **Frequency** | Kekerapan gelombang (1.0 - 3.0 Hz) ⭐ NEW |
|  - **Duration (sec)** | Durasi wave animation (0.0 - 60.0 detik, 0 = infinite) ⭐ NEW |
|  - **Step Animation Line** (Wave only) | Timing kapan wave animation berjalan: ⭐ NEW |
|  - **Before Polygon Draw** | Wave selesai sebelum polygons digambar |
|  - **With Polygon Draw** | Wave berjalan bareng polygon animation |
|  - **After Polygon Draw** | Wave berjalan setelah polygon selesai |
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
| **Right-Click (Original Dot)** | Context menu untuk Duplicate Dot (Above/Below/Left/Right/Track) |
| **Right-Click (UserDot)** | Context menu untuk Copy/Paste Color |
| **Right-Click (CustomLine/DcustomLine)** | Context menu untuk Duplicate Dot Track ⭐ NEW |
| **Mouse Scroll (Track Dot)** | Gerakkan dot sepanjang customLine path (highest priority) ⭐ NEW |
| **Mouse Scroll (Regular Dot)** | Gerakkan selected userDot sesuai arah offset (without CTRL) |
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

# Checkout branch sketch-islamic-gs-pltesselation
git checkout sketch-islamic-gs-pltesselation

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

### Duplicate Dot Track System ⭐ NEW

**UserDot Track Mode** - Dots yang bergerak沿着 customLine/DcustomLine path dengan scroll wheel.

**Creation Process (DuplicateManager::duplicateDotTrack):**
```cpp
// 1. Identify target line (hovered or selected)
int lineIndex = app->contextMenu->getHoveredLineIndex();
// Fallback to first selected line if no hovered line
if (lineIndex < 0 && app->selectionManager.hasSelectedLine()) {
    const std::set<int>& selectedIndices = app->selectionManager.getSelectedLineIndices();
    if (!selectedIndices.empty()) {
        lineIndex = *selectedIndices.begin();
    }
}

// 2. Calculate midpoint for initial dot position
const CustomLine& line = app->customLines[lineIndex];
const vector<vec2>& points = line.getPoints();
vec2 midpoint = (points[0] + points[1]) / 2.0f;

// 3. Create DotShape with track mode enabled
auto dotShape = std::make_unique<DotShape>(midpoint, "Dot", app->userDotRadius);
dotShape->setTrackLineIndex(lineIndex);  // ⭐ Link dot to line!
dotShape->setColor(app->colorManager->getUserDotColor());
dotShape->setLowerBound(lowerBound);  // Boundary based on line orientation
```

**Movement Logic (InputManager::handleMouseScrolled):**
```cpp
// 1. Get current position on curve (0.0 - 1.0)
float currentT = line.getClosestT(app->userDots[i]->getPosition());

// 2. Get curve length for consistent scroll speed
float curveLength = line.getApproxLength();

// 3. Convert pixel scroll to delta t
float scrollSpeed = 2.0f;
float scrollAmount = io.MouseWheel * scrollSpeed;
float deltaT = scrollAmount / curveLength;

// 4. Update t with BOUNDARY MARGIN ⭐ FIXED
// Track dots cannot reach the endpoints of the line
float trackTMargin = 0.05f;  // 5% margin at each end
float newT = ofClamp(currentT + deltaT, trackTMargin, 1.0f - trackTMargin);

// 5. Get new position on curve (supports bezier!)
vec2 newPos = line.getPointAt(newT);

// 6. Update dot position
app->userDots[i]->setPosition(newPos);

// 7. Update connected customLines (if any)
for (auto& customLine : app->customLines) {
    vector<vec2> linePoints = customLine.getPoints();
    for (size_t j = 0; j < linePoints.size(); j++) {
        if (glm::length(linePoints[j] - currentPos) < 0.1f) {
            linePoints[j] = newPos;  // Update connected point
            customLine.setPoints(linePoints);
        }
    }
}
```

**Curve Support (CustomLine):**
```cpp
// Get point at t (0.0 - 1.0) on quadratic bezier curve
vec2 CustomLine::getPointAt(float t) const {
    vec2 start = points[0];
    vec2 end = points[1];

    // Calculate control point based on curve parameter
    vec2 mid = (start + end) / 2.0f;
    vec2 dir = end - start;
    vec2 perpendicular = vec2(-dir.y, dir.x);  // Rotate 90°
    vec2 controlPoint = mid + perpendicular * curve;

    // Quadratic bezier formula: (1-t)²·P0 + 2(1-t)t·P1 + t²·P2
    return start * (1-t) * (1-t) +
           controlPoint * 2 * (1-t) * t +
           end * t * t;
}

// Find closest t value for a given point (for scroll tracking)
float CustomLine::getClosestT(vec2 point) const {
    // Sample curve at 100 points, find closest
    int samples = 100;
    float closestT = 0.0f;
    float minDist = FLOAT_MAX;

    for (int i = 0; i <= samples; i++) {
        float t = (float)i / samples;
        vec2 curvePoint = getPointAt(t);
        float dist = glm::length(point - curvePoint);

        if (dist < minDist) {
            minDist = dist;
            closestT = t;
        }
    }

    return closestT;
}

// Approximate curve length for scroll speed calculation
float CustomLine::getApproxLength() const {
    // Sample curve and sum distances
    int samples = 100;
    float length = 0.0f;
    vec2 prevPoint = getPointAt(0.0f);

    for (int i = 1; i <= samples; i++) {
        float t = (float)i / samples;
        vec2 currPoint = getPointAt(t);
        length += glm::length(currPoint - prevPoint);
        prevPoint = currPoint;
    }

    return length;
}
```

**Key Features:**
- **Bezier Support** - Dots follow curved lines, not just straight lines
- **Boundary Margin** - 5% margin prevents dots from reaching endpoints (configurable via `trackTMargin`)
- **Connected Lines** - Other lines connected to the dot update automatically
- **Scroll Priority** - Track dots have highest scroll priority (above regular dots, DcustomLines, curve adjustment)
- **Lower Bound System** - Boundary set based on line orientation (horizontal/vertical dominant)

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
│   ├── managers/              # Manager classes untuk core functionality
│   │   ├── ColorManager.cpp/h       # Color state & operations
│   │   ├── DuplicateManager.cpp/h   # Dot/Line duplication operations (Track mode!) ⭐ ACTIVE
│   │   ├── InputManager.cpp/h       # Mouse/keyboard input handling (~865 lines)
│   │   └── SelectionManager.cpp/h   # Selection state management
│   ├── shape/                # Shape implementations
│   │   ├── AbstractShape.cpp/h         # Base class untuk semua shapes (no showing flag)
│   │   ├── CircleShape.cpp/h           # Circle dengan angle/distance positioning
│   │   ├── CartesianAxes.cpp/h         # X-Y axes dengan scaling animation
│   │   ├── CrossLine.cpp/h             # Diagonal lines dengan proportional scaling
│   │   ├── ParallelogramLine.cpp/h     # Connecting lines dengan intersections
│   │   ├── RectangleLine.cpp/h         # Rectangle dengan 2 intersection dots
│   │   ├── OctagramLine.cpp/h          # 2-phase animation lines
│   │   ├── CustomLine.cpp/h            # User-created bezier lines (Track support) ⭐ UPDATED
│   │   ├── PolygonShape.cpp/h          # Fill-only polygons dengan animations
│   │   ├── DotShape.cpp/h              # Single dot shapes (Track mode) ⭐ UPDATED
│   │   └── DotInfo.h                   # Common struct untuk dot information
│   ├── anim/                 # Animation system
│   │   ├── AbstractAnimation.cpp/h    # Base class untuk animations
│   │   ├── FadeInAnimation.cpp/h      # Alpha fade effect dengan deltaTime
│   │   ├── WobbleAnimation.cpp/h      # Oscillation effect dengan deltaTime
│   │   ├── FillAnimation.cpp/h        # Water fill effect dengan deltaTime
│   │   ├── WobbleFillAnimation.cpp/h  # Wobble + fill combo ⭐ NEW
│   │   └── GradientAnimation.cpp/h    # Gradient flow effect ⭐ NEW
│   ├── template/             # Template system
│   │   ├── SacredGeometryTemplate.cpp/h  # Abstract template base
│   │   ├── TemplateRegistry.cpp/h        # Singleton registry
│   │   └── templates/
│   │       └── BasicZelligeTemplate.cpp/h # Moroccan pattern (26 shapes)
│   ├── operation/            # Operations layer
│   │   ├── FileManager.cpp/h       # .nay save/load dengan speed sync
│   │   ├── FileOperationManager.cpp/h # File operations wrapper
│   │   └── gui/                    # ImGui components
│   │       ├── AbstractGuiComponent.cpp/h # GUI base
│   │       ├── MenuBar.cpp/h            # File/Edit/View menus
│   │       ├── SacredGeometry.cpp/h     # Template control panel (Transform UI)
│   │       ├── Playground.cpp/h         # Playback panel (Draw Settings UI)
│   │       ├── UserCustom.cpp/h         # User control panel
│   │       ├── ContextMenu.cpp/h        # Right-click context menu
│   │       ├── SuccessPopup.cpp/h       # Success dialog
│   │       ├── ErrorPopup.cpp/h         # Error dialog
│   │       ├── ConfirmationPopup.cpp/h  # Confirmation dialog
│   │       ├── SelectionInfo.cpp/h      # Selected objects info window
│   │       └── ObjectTooltip.cpp/h      # Object tooltips manager
│   ├── undo/                # Undo/Redo system
│   │   └── UndoAction.h             # Undo/Redo action definitions
│   └── utils/               # Utility functions
│       └── GeometryUtils.cpp/h       # Geometry helper functions
├── bin/                      # Compiled executable & data
│   └── data/                 # Saved workspaces (.nay files)
├── imgui/                    # ImGui library integration
├── README.md                 # Comprehensive documentation (this file)
└── BasicIslamicGeometry.sln  # Visual Studio solution
```

**Total Files**: 70+ source files (.cpp + .h)

**Architecture Highlights:**
- **Manager Pattern**: Centralized managers (Color, Duplicate, Input, Selection) untuk clean separation of concerns
- **Template System**: SacredGeometryTemplate base class untuk extensibility
- **Template Registry**: Singleton pattern untuk managing patterns
- **GUI System**: Modular ImGui components dengan AbstractGuiComponent
- **Shape Hierarchy**: Semua shapes inherit dari AbstractShape (no show/hide)
- **Animation System**: AbstractAnimation base untuk reusable animations dengan deltaTime (FadeIn, Wobble, Fill, WobbleFill, Gradient)
- **Speed Control**: Centralized speed multiplier system untuk semua animations
- **GLSL Rendering**: Conditional GPU/CPU rendering untuk polygons berdasarkan `loadedFromFile`
- **FileOperationManager**: Wrapper pattern untuk file operations (mirip ColorManager)
- **Conditional UI**: CollapsingHeader hanya muncul jika file punya data
- **Skip Load**: CustomLines load bisa di-skip via checkbox
- **Object Tooltip System**: Custom OF rendering untuk selected objects info
- **UserDot System**: Flexible dot placement dengan 5 mode (Above, Below, Left, Right, Track) ⭐ NEW
- **Track Mode**: Dots bergerak沿 customLine/DcustomLine dengan boundary margin ⭐ NEW
- **Color Management**: Smart sync antara objects dan color pickers
- **Undo/Redo**: 100-step history dengan comprehensive state tracking (termasuk CREATE_DOT radius)
- **File Operations**: FileOperationManager dengan direct file save dan speed sync
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
- 🎭 GLSL shaders untuk advanced polygon rendering
- 🗂️ File operation manager pattern untuk clean architecture
- 🎯 Track mode system untuk interactive dots沿 curves ⭐ NEW

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

---

## 📝 Current Status: **sketch-islamic-gs-tesselation**

Branch ini adalah **Islamic Geometry Studio** - aplikasi komprehensif untuk membuat, mengedit, dan menyimpan pola geometri Islam dengan GUI berbasis ImGui, sistem template yang modular, speed control global, transform canvas, draw only concept, object tooltips, userDot system (termasuk track mode), GLSL rendering, file operation manager, dan **TESSellation Canvas system**.

### Fitur Terbaru: **Tessellation Canvas - Template Tessellation (Synchronous)** ⭐ NEW (2026-03-05)

**Tessellation Canvas System** - Copy pattern geometri ke grid di seluruh canvas:
- **Square Grid System** - Tiles dengan ukuran tessellationRadius × 2
- **Inverse Canvas Transform** - Grid positioning yang akurat dengan memperhitungkan pan, rotate, zoom
- **Direct Rendering** - Smooth anti-aliased lines (tanpa FBO cache, direct OpenGL dengan MSAA)
- **Seamless Tiling** - Tiles bersebelahan tanpa overlap, tidak ada double lines di boundaries
- **Synchronous Mode** - Semua tiles animate bersamaan (template parallel sync)
- **Performance Optimized** - Solid 60 FPS dengan 165 tiles × 26 shapes

**Technical Implementation:**
- `TessellationManager` class - Square grid calculation dengan inverse transform
- World bounds calculation untuk accurate viewport coverage
- Direct OpenGL rendering (seperti polygon tessellation) untuk quality terbaik
- Grid generation tanpa margin untuk cegah overlap artifacts

**Modified Files:**
- `src/managers/TessellationManager.cpp/h` - Grid calculation, inverse transform, origin alignment
- `src/ofApp.cpp/h` - Tessellation integration, draw logic, state management
- `BasicIslamicGeometry.vcxproj/filters` - TessellationManager registration

### Fitur Terbaru: **Pause Duration for Special Animation** ⭐ NEW (2026-03-02)

Perubahan terbaru yang telah selesai:

- **Pause Duration Control** - Slider Pause (0.0 - 1.0 detik) untuk semua Special Animation:
  - **Orbit Left/Right** - Polygon pause di posisi maxAngle, lalu kembali ke posisi awal
  - **Spin Left/Right** - Polygon pause di posisi maxAngle, lalu kembali ke posisi awal
  - Slider hanya muncul ketika Orbit atau Spin animation dipilih
  - Default: 0.0 detik (tidak ada pause)
  - Pause berlaku setelah polygon mencapai posisi maxAngle dalam animasi

- **Phase-Based Animation System** - Implementasi phase-based animation untuk pause:
  - **FORWARD phase** - Polygon bergerak dari 0° ke ±maxAngle
  - **PAUSED phase** - Polygon pause di posisi maxAngle sesuai durasi yang ditentukan
  - **BACKWARD phase** - Polygon kembali dari ±maxAngle ke 0°
  - Animation menggunakan half-cosine wave untuk osilasi halus

- **Per-Polygon Pause Configuration** - Setiap polygon dapat memiliki durasi pause yang berbeda:
  - Pause duration disimpan di `specialPolygonPauseDurations` vector
  - Vector di-clear saat file close
  - Pause duration diterapkan saat tombol Draw diklik (tidak real-time)

- **Modified Files**:
  - `SpinLeftAnimation.cpp/h` - Tambah pause system (phase enum, pauseTimer, isPaused flag, setPauseDuration method)
  - `SpinRightAnimation.cpp/h` - Tambah pause system (sama seperti SpinLeft)
  - `RotateLeftAnimation.cpp/h` - Tambah pause system (Orbit Left)
  - `RotateRightAnimation.cpp/h` - Tambah pause system (Orbit Right)
  - `BasicZelligeTemplate.cpp/h` - Tambah `specialPolygonPauseDurations` vector, UI slider Pause untuk Orbit/Spin animations
  - `FileManager.cpp/h` - Update `applySpecialPolygonAnimations` dengan parameter `pauseDurations`, pass pause duration ke semua 4 animation types
  - `FileOperationManager.cpp` - Initialize `specialPolygonPauseDurations` saat load file
  - `ofApp.cpp` - Update pemanggilan `applySpecialPolygonAnimations` dengan pause durations

---

### Fitur Sebelumnya: **UI Improvements & Special Speed Control** (2026-02-28)

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
