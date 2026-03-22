## Supported boards

Soc | Boards |
|:--|:--|
| Allwinner H6 | Orange Pi 3/3 LTS |
| Allwinner H616 | Orange Pi Zero2/Zero2w/Zero3 | 
| Allwinner T527 | Orange Pi 4A |
| Allwinner A733 | Orange Pi 4Pro | 
| Rockchip RK3399 | Orange Pi 4/4B/4 LTS/800 |
| Rockchip RK3566 | Orange Pi 3B/CM4 |
| Rockchip RK3588S | Orange Pi 5/5B/5Pro/CM5/CM5-tablet |
| Rockchip RK3588 | Orange Pi 5Plus/5MAX/5Ultra |
| Cix P1 | Orange Pi 6Plus |
| Starfive  JH7110 | Orange Pi RV |
| Ky X1 | Orange Pi RV2/R2S |

## Download links

- 中文链接：     http://www.orangepi.cn
- English link：http://www.orangepi.org

## Supported Host Systems
- Ubuntu 22.04

## GitHub Actions (Orange Pi Zero 2)

Workflow `.github/workflows/orangepi-zero2-images.yml` khusus untuk **Orange Pi Zero 2** dan bisa dijalankan manual lewat **workflow_dispatch** atau otomatis saat ada **push/pull request** ke `main/master` (untuk path terkait build).

Output yang tersedia:
- Debian Bullseye (CLI image)
- Debian Bookworm (CLI image)
- Ubuntu Jammy (CLI image)
- Arch Linux ARM (aarch64 rootfs artifact)

Cara pakai singkat:
1. Buka tab **Actions** -> **Build Orange Pi Zero 2 Images**.
2. Klik **Run workflow**.
3. (Opsional) isi input `releases` dengan `all` atau daftar rilis dipisah koma, contoh: `bookworm,jammy`.
   - Untuk Arch Linux ARM saja, gunakan `arch`.
4. (Opsional) ubah `artifact_retention_days` jika perlu.
5. Download artifact hasil job setelah workflow selesai.

> Catatan: build image native di source tree ini saat ini untuk distro Debian/Ubuntu. Untuk Arch, workflow menyiapkan rootfs `aarch64` dan memverifikasi checksum SHA256 dari sumber resmi sebelum upload artifact.

### Perlu isi GitHub Variables?
Tidak wajib. Workflow ini sudah punya default dan langsung jalan tanpa isi variable apa pun.

Opsional kalau mau kustom:
- `ORANGEPI_BOARD` (default: `orangepizero2`)
- `ORANGEPI_COMPRESS` (default: `sha,img`)
- `ARCHLINUXARM_ROOTFS_URL` (default: `https://mirror.archlinuxarm.org/os/ArchLinuxARM-aarch64-latest.tar.gz`)
