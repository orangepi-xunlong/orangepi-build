d-#!/bin/bash

# OrangePi Docker 构建脚本
# 此脚本用于在Docker容器中构建OrangePi系统

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查是否安装了Docker
check_docker() {
    if ! command -v docker &> /dev/null; then
        print_error "Docker 未安装，请先安装Docker"
        exit 1
    fi
    
    # 检查Docker服务是否运行
    if ! docker info &> /dev/null; then
        print_error "Docker 服务未运行，请启动Docker服务"
        exit 1
    fi
    
    print_success "Docker 检查通过"
}

# 检查是否在Linux环境下运行
check_environment() {
    OS=$(uname -s)
    if [ "$OS" != "Linux" ]; then
        print_warning "宿主机系统: $OS"
        print_warning "Docker Desktop 将在 Linux VM 中运行容器"
        print_info "Loop 设备和镜像打包功能在 Docker Desktop 中可用"
    else
        print_success "运行环境检查通过 (Linux)"
    fi
    return 0
}

# 构建Docker镜像
build_image() {
    print_info "开始构建Docker镜像..."
    
    # 获取脚本所在目录
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    
    # 构建Docker镜像，指定platform为amd64
    docker build --platform linux/amd64 -t orangepi-build:latest "$SCRIPT_DIR"
    
    if [ $? -eq 0 ]; then
        print_success "Docker镜像构建成功"
    else
        print_error "Docker镜像构建失败"
        exit 1
    fi
}

# 在Docker容器中运行构建
run_build() {
    print_info "在Docker容器中运行OrangePi构建..."
    
    # 获取脚本所在目录
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    
    # 过滤掉docker-build.sh自己的选项，只保留传递给build.sh的参数
    BUILD_PARAMS=()
    for arg in "$@"; do
        case $arg in
            --build-image|--help|-h)
                # 跳过docker-build.sh自己的选项
                ;;
            *)
                BUILD_PARAMS+=("$arg")
                ;;
        esac
    done
    
    # 运行Docker容器，指定platform为amd64
    # 使用Docker volume保存构建产物和工具链
    # 使用--privileged提供必要的权限用于loop设备操作
    # 使用-u root确保以root用户运行
    
    # 检测操作系统类型
    local HOST_OS=$(uname -s)
    
    # 如果存在同名容器，先删除
    docker rm -f orangepi-build 2>/dev/null || true
    
    # 构建基础 Docker 参数
    local DOCKER_ARGS=(
        --platform linux/amd64
        -it
        --privileged
        -u root
        --device /dev/loop-control
        --cap-add SYS_ADMIN
        --cap-add MKNOD
    )
    
    # 关于 /dev 挂载:
    # - Linux: 可以直接挂载宿主机的 /dev
    # - macOS/Windows (Docker Desktop): 不应该挂载 /dev，因为:
    #   1. macOS 的 /dev 是 BSD 格式，与 Linux 不兼容
    #   2. Windows 没有 /dev 概念
    #   3. Docker Desktop 的 Linux VM 已经有完整的 /dev
    #   4. --privileged 已经提供了足够的权限
    if [ "$HOST_OS" = "Linux" ]; then
        DOCKER_ARGS+=(-v /dev:/dev)
        print_info "检测到 Linux 系统，已启用 /dev 挂载"
    else
        print_info "检测到 $HOST_OS 系统，使用 Docker Desktop Linux VM 的 /dev"
        print_info "--privileged 模式已提供 loop 设备支持"
    fi
    
    # 添加其他 volume 挂载
    DOCKER_ARGS+=(
        -v ./orangepi-output:/root/orangepi/output
        -v orangepi-toolchains:/root/orangepi/toolchains
        -v orangepi-kernel:/root/orangepi/kernel
        -v orangepi-u-boot:/root/orangepi/u-boot
        -v orangepi-tmp:/root/orangepi/.tmp
        -v orangepi-sources:/root/orangepi/external/cache/sources
        -w /root/orangepi
        -e LANG=en_US.UTF-8
        -e LANGUAGE=en_US:en
        -e LC_ALL=en_US.UTF-8
        -e TERM=xterm-256color
        -e COLUMNS=120
        -e LINES=30
        -e NO_APT_CACHER=yes
        --name orangepi-build
    )
    
    if [ ${#BUILD_PARAMS[@]} -eq 0 ]; then
        # 如果没有参数，启动交互式shell
        docker run "${DOCKER_ARGS[@]}" orangepi-build:latest
    else
        # 如果有参数，执行构建
        docker run "${DOCKER_ARGS[@]}" orangepi-build:latest "${BUILD_PARAMS[@]}"
    fi
}

# 显示帮助信息
show_help() {
    echo "OrangePi Docker 构建脚本"
    echo ""
    echo "用法: $0 [选项] [构建参数]"
    echo ""
    echo "选项:"
    echo "  --build-image    仅构建Docker镜像"
    echo "  --help           显示此帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                    # 构建镜像并运行默认构建"
    echo "  $0 --build-image      # 仅构建Docker镜像"
    echo "  $0 BOARD=orangepizero2 BRANCH=current RELEASE=jammy BUILD_MINIMAL=yes"
    echo ""
    echo "注意: 构建参数将直接传递给OrangePi的build.sh脚本"
}

# 主函数
main() {
    # 解析命令行参数
    BUILD_IMAGE_ONLY=false
    
    for arg in "$@"; do
        case $arg in
            --build-image)
                BUILD_IMAGE_ONLY=true
                shift
                ;;
            --help|-h)
                show_help
                exit 0
                ;;
            *)
                # 保留其他参数用于构建
                ;;
        esac
    done
    
    print_info "开始OrangePi Docker构建流程"
    
    # 检查环境
    check_docker
    check_environment
    
    # 构建Docker镜像
    build_image

    # 如果只需要构建镜像，则退出
    if [ "$BUILD_IMAGE_ONLY" = true ]; then
        print_success "Docker镜像构建完成"
        exit 0
    fi
    
    # 运行构建
    run_build "$@"
    
    print_success "OrangePi构建完成"
}

# 执行主函数
main "$@"