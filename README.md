# CREPL
C-flavor Read Evalute Print Line

## build
```
sudo apt install libedit-dev
make
```

## run
```
./crepl
```

## usage

### Evalute equation
式を評価するには、プロンプトに続き式を入力する。
```
>> 1 + 2 + 3
(0x00000006) 6
```
数値として使用可能な表現は、
- `123`などの、10進数表記の数値 `^[0-9]+`
- `0xFF`などの、16進数表記の数値 `^0[xX][0-9a-fA-F]+`
- `0b1101`などの、2進数表記の数値 `0[bB][0-1]+`

### Assign variable
変数への代入は、変数に代入演算子'='で代入する。
```
>> a = 1 + 2
(0x00000003) 3
```
変数として使用可能な文字列は、予約語を除く、
- `^[a-zA-Z][a-zA-Z0-9]*`

### Show variable
変数の表示は、変数を評価すれば良い。
```
>> a
(0x00000003) 3
```

### print all variable
全ての変数を表示する。
```
>> print
```
### exit program
```
>> exit
```

## unittest(gtest)
```
git submodule init
git submodule update
make test
```
