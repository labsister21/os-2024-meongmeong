# MeongOS ~nyawww~~~ o(*￣▽￣*)ブ
> Tugas Besar IF2230 ~waku ~waku
<p>Sistem Operasi 32-bit yang dibuat dengan cinta きです </p>

## Project Status
| Checkpoint | Status |
| ---------- | :------: |
| Milestone 0 |  ✅  |
| Milestone 1 |  ✅  |
| Milestone 2 |  ✅  |
| Milestone 3 |  ✅  |

## Milestone
### Milestone 0
- Kernel dasar
- Otomatisasi build
- Menjalankan sistem operasi
- Menjalankan OS dengan QEMU
- Pembuatan struktur data GDT
- Load GDT

### Milestone 1
- Text framebuffer
- Interrupt
- Keyboard driver
- Filesystem

### Milestone 2
- Manajemen memory
- Separasi kernel-user space
- Shell

### Milestone 3
- Menyiapkan struktur untuk proses
- Membuat task scheduler & context switch
- Membuat perintah shell tambahan untuk manajemen proses
- Menjalankan multitasking

## Kebutuhan
Untuk menjalankan OS, dibutuhkan beberapa program berikut:
- Netwide assembler
- GNU C Compiler
- QEMU - System i386
- GNU Make
- genisoimage
- GNU Debugger

Program bisa didapatkan pada Ubuntu 20.04 dengan perintah berikut:
```
sudo apt update
sudo apt install -y nasm gcc qemu-system-x86 make genisoimage gdb
```

## Cara Menjalankan
Anda dapat langsung menggunakan VSCode untuk menjalankan atau debug dengan mengikuti langkah-langkah berikut:
1. Pastikan Anda berada di folder root
2. Jika ini pertama kalinya Anda menjalankan aplikasi atau Anda ingin membuat penyimpanan baru untuk sistem file, jalankan perintah ini:

```
make disk
```
3. Masukkan program pengguna ke dalam sistem dengan perintah ini:
```
make insert-shell
make insert-clock
```
4. Jalankan program dengan perintah
```
make run
```
Atau jika ingin menjalankan semua perintah di atas sekaligus, Anda dapat menjalankan
```
make all-and-insert
```

## Authors
| Name                            | GitHub                                           | NIM      |  Contact                     |
| ------------------------------ | ------------------------------------------------- | -------- | ---------------------------- |
| Amalia Putri                   | [amaliap21](https://github.com/amaliap21)   | 13522025 | 13522042@std.stei.itb.ac.id  |
| Angelica Kierra Ninta Gurning  | [angiekierra](https://github.com/angiekierra)     | 13522048 | 13522048@std.stei.itb.ac.id  |
| Imanuel Sebastian Girsang      | [ImanuelSG](https://github.com/ImanuelSG)             | 13522058 | 13522058@std.stei.itb.ac.id  |
| Julian Chandra Sutadi          | [julianchandras](https://github.com/julianchandras)             | 13522058 | 13522080@std.stei.itb.ac.id  |
