
extern ISR_interrupt_service_routine

ISR_common:
    ; pushes eax, ecx, edx, ebx, esp, ebp, esi, edi
    pusha

    ; push ds
    mov eax, 0
    mov ax, ds
    push eax

    ; switch to kernal data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp ; pass pointer to stack
    call ISR_interrupt_service_routine
    add esp, 4 ; pop esp

    ; restore old data segments
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8 ; pop interrupt number and error code
    iret

%macro ISR_stub_no_error__ 1
global ISR_stub_%1
ISR_stub_%1:
    push 0 ; error code = 0
    push %1
    jmp ISR_common
%endmacro

%macro ISR_stub_with_error 1
global ISR_stub_%1
ISR_stub_%1:
    push 0 ; error code = 0
    push %1
    jmp ISR_common
%endmacro

ISR_stub_no_error__ 0
ISR_stub_no_error__ 1
ISR_stub_no_error__ 2
ISR_stub_no_error__ 3
ISR_stub_no_error__ 4
ISR_stub_no_error__ 5
ISR_stub_no_error__ 6
ISR_stub_no_error__ 7
ISR_stub_no_error__ 8 ; spec: error always zero but is not pushed 
ISR_stub_no_error__ 9
ISR_stub_with_error 10
ISR_stub_with_error 11
ISR_stub_with_error 12
ISR_stub_with_error 13
ISR_stub_with_error 14
ISR_stub_no_error__ 15
ISR_stub_no_error__ 16
ISR_stub_with_error 17
ISR_stub_no_error__ 18
ISR_stub_no_error__ 19
ISR_stub_no_error__ 20
ISR_stub_with_error 21
ISR_stub_no_error__ 22
ISR_stub_no_error__ 23
ISR_stub_no_error__ 24
ISR_stub_no_error__ 25
ISR_stub_no_error__ 26
ISR_stub_no_error__ 27
ISR_stub_no_error__ 28
ISR_stub_with_error 29
ISR_stub_with_error 30
ISR_stub_no_error__ 31
ISR_stub_no_error__ 32
ISR_stub_no_error__ 33
ISR_stub_no_error__ 34
ISR_stub_no_error__ 35
ISR_stub_no_error__ 36
ISR_stub_no_error__ 37
ISR_stub_no_error__ 38
ISR_stub_no_error__ 39
ISR_stub_no_error__ 40
ISR_stub_no_error__ 41
ISR_stub_no_error__ 42
ISR_stub_no_error__ 43
ISR_stub_no_error__ 44
ISR_stub_no_error__ 45
ISR_stub_no_error__ 46
ISR_stub_no_error__ 47
ISR_stub_no_error__ 48
ISR_stub_no_error__ 49
ISR_stub_no_error__ 50
ISR_stub_no_error__ 51
ISR_stub_no_error__ 52
ISR_stub_no_error__ 53
ISR_stub_no_error__ 54
ISR_stub_no_error__ 55
ISR_stub_no_error__ 56
ISR_stub_no_error__ 57
ISR_stub_no_error__ 58
ISR_stub_no_error__ 59
ISR_stub_no_error__ 60
ISR_stub_no_error__ 61
ISR_stub_no_error__ 62
ISR_stub_no_error__ 63
ISR_stub_no_error__ 64
ISR_stub_no_error__ 65
ISR_stub_no_error__ 66
ISR_stub_no_error__ 67
ISR_stub_no_error__ 68
ISR_stub_no_error__ 69
ISR_stub_no_error__ 70
ISR_stub_no_error__ 71
ISR_stub_no_error__ 72
ISR_stub_no_error__ 73
ISR_stub_no_error__ 74
ISR_stub_no_error__ 75
ISR_stub_no_error__ 76
ISR_stub_no_error__ 77
ISR_stub_no_error__ 78
ISR_stub_no_error__ 79
ISR_stub_no_error__ 80
ISR_stub_no_error__ 81
ISR_stub_no_error__ 82
ISR_stub_no_error__ 83
ISR_stub_no_error__ 84
ISR_stub_no_error__ 85
ISR_stub_no_error__ 86
ISR_stub_no_error__ 87
ISR_stub_no_error__ 88
ISR_stub_no_error__ 89
ISR_stub_no_error__ 90
ISR_stub_no_error__ 91
ISR_stub_no_error__ 92
ISR_stub_no_error__ 93
ISR_stub_no_error__ 94
ISR_stub_no_error__ 95
ISR_stub_no_error__ 96
ISR_stub_no_error__ 97
ISR_stub_no_error__ 98
ISR_stub_no_error__ 99
ISR_stub_no_error__ 100
ISR_stub_no_error__ 101
ISR_stub_no_error__ 102
ISR_stub_no_error__ 103
ISR_stub_no_error__ 104
ISR_stub_no_error__ 105
ISR_stub_no_error__ 106
ISR_stub_no_error__ 107
ISR_stub_no_error__ 108
ISR_stub_no_error__ 109
ISR_stub_no_error__ 110
ISR_stub_no_error__ 111
ISR_stub_no_error__ 112
ISR_stub_no_error__ 113
ISR_stub_no_error__ 114
ISR_stub_no_error__ 115
ISR_stub_no_error__ 116
ISR_stub_no_error__ 117
ISR_stub_no_error__ 118
ISR_stub_no_error__ 119
ISR_stub_no_error__ 120
ISR_stub_no_error__ 121
ISR_stub_no_error__ 122
ISR_stub_no_error__ 123
ISR_stub_no_error__ 124
ISR_stub_no_error__ 125
ISR_stub_no_error__ 126
ISR_stub_no_error__ 127
ISR_stub_no_error__ 128
ISR_stub_no_error__ 129
ISR_stub_no_error__ 130
ISR_stub_no_error__ 131
ISR_stub_no_error__ 132
ISR_stub_no_error__ 133
ISR_stub_no_error__ 134
ISR_stub_no_error__ 135
ISR_stub_no_error__ 136
ISR_stub_no_error__ 137
ISR_stub_no_error__ 138
ISR_stub_no_error__ 139
ISR_stub_no_error__ 140
ISR_stub_no_error__ 141
ISR_stub_no_error__ 142
ISR_stub_no_error__ 143
ISR_stub_no_error__ 144
ISR_stub_no_error__ 145
ISR_stub_no_error__ 146
ISR_stub_no_error__ 147
ISR_stub_no_error__ 148
ISR_stub_no_error__ 149
ISR_stub_no_error__ 150
ISR_stub_no_error__ 151
ISR_stub_no_error__ 152
ISR_stub_no_error__ 153
ISR_stub_no_error__ 154
ISR_stub_no_error__ 155
ISR_stub_no_error__ 156
ISR_stub_no_error__ 157
ISR_stub_no_error__ 158
ISR_stub_no_error__ 159
ISR_stub_no_error__ 160
ISR_stub_no_error__ 161
ISR_stub_no_error__ 162
ISR_stub_no_error__ 163
ISR_stub_no_error__ 164
ISR_stub_no_error__ 165
ISR_stub_no_error__ 166
ISR_stub_no_error__ 167
ISR_stub_no_error__ 168
ISR_stub_no_error__ 169
ISR_stub_no_error__ 170
ISR_stub_no_error__ 171
ISR_stub_no_error__ 172
ISR_stub_no_error__ 173
ISR_stub_no_error__ 174
ISR_stub_no_error__ 175
ISR_stub_no_error__ 176
ISR_stub_no_error__ 177
ISR_stub_no_error__ 178
ISR_stub_no_error__ 179
ISR_stub_no_error__ 180
ISR_stub_no_error__ 181
ISR_stub_no_error__ 182
ISR_stub_no_error__ 183
ISR_stub_no_error__ 184
ISR_stub_no_error__ 185
ISR_stub_no_error__ 186
ISR_stub_no_error__ 187
ISR_stub_no_error__ 188
ISR_stub_no_error__ 189
ISR_stub_no_error__ 190
ISR_stub_no_error__ 191
ISR_stub_no_error__ 192
ISR_stub_no_error__ 193
ISR_stub_no_error__ 194
ISR_stub_no_error__ 195
ISR_stub_no_error__ 196
ISR_stub_no_error__ 197
ISR_stub_no_error__ 198
ISR_stub_no_error__ 199
ISR_stub_no_error__ 200
ISR_stub_no_error__ 201
ISR_stub_no_error__ 202
ISR_stub_no_error__ 203
ISR_stub_no_error__ 204
ISR_stub_no_error__ 205
ISR_stub_no_error__ 206
ISR_stub_no_error__ 207
ISR_stub_no_error__ 208
ISR_stub_no_error__ 209
ISR_stub_no_error__ 210
ISR_stub_no_error__ 211
ISR_stub_no_error__ 212
ISR_stub_no_error__ 213
ISR_stub_no_error__ 214
ISR_stub_no_error__ 215
ISR_stub_no_error__ 216
ISR_stub_no_error__ 217
ISR_stub_no_error__ 218
ISR_stub_no_error__ 219
ISR_stub_no_error__ 220
ISR_stub_no_error__ 221
ISR_stub_no_error__ 222
ISR_stub_no_error__ 223
ISR_stub_no_error__ 224
ISR_stub_no_error__ 225
ISR_stub_no_error__ 226
ISR_stub_no_error__ 227
ISR_stub_no_error__ 228
ISR_stub_no_error__ 229
ISR_stub_no_error__ 230
ISR_stub_no_error__ 231
ISR_stub_no_error__ 232
ISR_stub_no_error__ 233
ISR_stub_no_error__ 234
ISR_stub_no_error__ 235
ISR_stub_no_error__ 236
ISR_stub_no_error__ 237
ISR_stub_no_error__ 238
ISR_stub_no_error__ 239
ISR_stub_no_error__ 240
ISR_stub_no_error__ 241
ISR_stub_no_error__ 242
ISR_stub_no_error__ 243
ISR_stub_no_error__ 244
ISR_stub_no_error__ 245
ISR_stub_no_error__ 246
ISR_stub_no_error__ 247
ISR_stub_no_error__ 248
ISR_stub_no_error__ 249
ISR_stub_no_error__ 250
ISR_stub_no_error__ 251
ISR_stub_no_error__ 252
ISR_stub_no_error__ 253
ISR_stub_no_error__ 254
ISR_stub_no_error__ 255