# Computação Gráfica Básica em C

Esse repositório contém alguns algoritmos clássicos de Computação Gráfica implementados em C. Esse projeto é parte da cadeira de Computação Gráfica da UFRPE (BCC, 2023.1). Especificamente, esse repositório contém uma pipeline de renderização simples baseada no *scanline* e o modelo de reflexão e tonalização de Phong. Para execução da pipeline, são necessários os arquivos que descrevem: (i) a câmera virtual; (ii) a malha do objeto 3D; e (iii) a iluminação da cena.

## Quickstart

Para testar os resultados obtidos, basta realizar o download do [executável mais recente](https://github.com/moesio-f/classic-cg/releases) com seu sistema operacional (Windows ou Linux) e extrair os arquivos presentes no `zip`. Uma outra opção é compilar o projeto manualmente (necessário o SDL2, CMake e Make) utilizando o Makefile e CMake.

Uma vez realizado obtido o executável, podemos testar a pipeline de renderização da seguinte forma:

```console
# Executando em sistemas Linux-like
# ./render <path/to/camera.txt> </path/to/object.byu>
./render camera_1.txt calice2.byu basic.lux    

# Executando no Windows
# render.exe <path/to/camera.txt> </path/to/object.byu>
render.exe camera_1.txt calice2.byu basic.lux    
```

Ao executar as linhas acima, será produzido como resultado a seguinte visualização:

![](.github/img/2va_calice.png)

Podemos atualizar os arquivos de câmera (`.txt`) e de objeto (`.byu`) e apertar a tecla `R` para reexecutar o processo de renderização.

## Arquivo de descrição da Câmera

O arquivo de descrição da câmera possui os parâmetros da câmera virtual a serem utilizadas no processo de renderização. A tabela a seguir contém a descrição de cada um desses parâmetros.

| Parâmetro | Descrição |
| --- | --- |
| `C` | vetor, representa o ponto de foco. | 
| `N` | vetor, base ortonormal do sistema. |
| `V` | vetor, base ortonormal do sistema. |
| `d` | escalar, define a distância do foco ao plano de vista. |
| `hx`| escalar, determina o retângulo de vista. |
| `hy`| escalar, determina o retângulo de vista. |


Um exemplo para esse arquivo é:

```
C = 0 -500 500
N = 0 1 -1
V = 0 -1 -1
d = 5
hx = 2
hy = 2
```

## Arquivo de descrição do Objeto

Esse é um arquivo que descreve a malha 3D do objeto a ser renderizado. Nele, são definidos todos os triângulos em coordenadas globais e como eles se conectam para formar o objeto. O formato desse arquivo é `.byu` e ele deve conter os seguintes dados:

```
<no de vértices> <no de triângulos>
<coordenada x do vértice 1> <coordenada y do vértice 1> <coordenada z do vértice 1>
<coordenada x do vértice 2> <coordenada y do vértice 2> <coordenada z do vértice 2>
...
<coordenada x do vértice n> <coordenada y do vértice n> <coordenada z do vértice n>
<índice do vértice 1 do triângulo 1> <índice do vértice 2 do triângulo 1> <índice do vértice 3 do triângulo 1>
<índice do vértice 1 do triângulo 2> <índice do vértice 2 do triângulo 2> <índice do vértice 3 do triângulo 2>
...
<índice do vértice 1 do triângulo k> <índice do vértice 2 do triângulo k> <índice do vértice 3 do triângulo k>
```

## Arquivo de descrição de Iluminação

Esse é um arquivo que define os parâmetros de iluminação (ambiente, difusa e especular) para a cena e o objeto 3D. O formato desse arquivo é `.lux` e ele deve conter os seguintes parâmetros:

| Parâmetro | Descrição |
| --- | --- |
| `Iamb` | vetor, representa a cor da luz ambiente. | 
| `Ka` | escalar, coeficiente de reflexão ambiental. |
| `Il` | cor da luz incidente da componente difusa. |
| `Pl` | vetor, posição da fonte de luz. |
| `Kd`| vetor, coeficiente de reflexão difusa. |
| `Od`| vetor, cor difusa da superfície do objeto. |
| `Ks`| escalar, coeficiente de reflexão especular do objeto. |
| `eta`| escalar, modela o tamanho do destaque especular. |


Um exemplo para esse arquivo é:

```
Iamb = 100 100 100
Ka = 0.2
Il = 127 213 254
Pl = 60 5 -10
Kd = 0.5 0.3 0.2
Od = 0.7 0.5 0.8
Ks = 0.5
eta = 1
```

## Compilando o projeto (Linux)

Também é possível realizar o clone do repositório e compilar o projeto (~15s) para geração do executável. Para realizar a compilação, é necessário:

- SDL2: https://wiki.archlinux.org/title/SDL;
- CMake (3.20+): https://cmake.org/;
- Make;

Uma vez que todas as dependências estejam instaladas, podemos fazer:

```console
git clone https://github.com/moesio-f/classic-cg/
cd classic-cg
make
./build/render data/camera/camera_1.txt data/objects/maca2.byu

# Para realização o cross-compile para windows
# precisamos do mingw64 e do SDL2 para o mingw64
# após isso, podemos fazer:
# make compile-windows
```
