.486
.model flat,stdcall
option casemap:none
;include \masm32\include\windows.inc
;include \masm32\include\kernel32.inc
;includelib \masm32\lib\kernel32.lib

.code

XTEA PROC Encrypt:BYTE,Key:DWORD,Packet:DWORD,XTEAPacket:DWORD
	LOCAL XTEAkeyPointer		:DWORD
	LOCAL NumberOfBlocks		:WORD
	LOCAL PacketPointer			:DWORD
	LOCAL XTEAPacketPointer		:DWORD

	.IF Key!=0
		;Fix pointers
		mov eax,Key
		mov XTEAkeyPointer,eax
		mov eax,Packet
		mov ecx,XTEAPacket
		mov PacketPointer,eax
		mov XTEAPacketPointer,ecx
		
		;Calculate how many blocks of 8-bytes that is going to be encrypted/decrypted
		movzx eax,WORD PTR [eax]
		.IF Encrypt!=0
			
			add eax,2
			mov cx,8
			xor edx,edx
			div cx
			.IF edx!=0
				inc ax
			.ENDIF
			mov NumberOfBlocks,ax
			
			;Calculate the length of the encrypted packet and also write the header
			lea eax,[eax*8]
			mov ecx,XTEAPacketPointer
			mov WORD PTR [ecx],ax
			add XTEAPacketPointer,2
			
		.ELSEIF Encrypt==0
			
			shr eax,3
			mov NumberOfBlocks,ax
			
			add PacketPointer,2
			
		.ENDIF
		
		movzx eax,Encrypt
		push eax
		
		;Main encryption loop
		.REPEAT
			mov edx,PacketPointer
			.IF BYTE PTR [esp]!=0
				xor eax,eax
				mov ecx,DWORD PTR [edx]
				mov edx,DWORD PTR [edx+4]
			.ELSEIF BYTE PTR [esp]==0
				mov eax,0C6EF3720h
				mov ecx,DWORD PTR [edx+4]
				mov edx,DWORD PTR [edx]
			.ENDIF
			push ebx
			push ebp
			push esi
			mov esi,XTEAkeyPointer
			push edi
			mov edi,32
			.REPEAT
				MOV EBX,EDX
				SHR EBX,5
				MOV EBP,EDX
				SHL EBP,4
				XOR EBX,EBP
				ADD EBX,EDX
				MOV EBP,EAX
				.IF BYTE PTR [esp+4*SIZEOF DWORD]==0
					SHR	EBP,0Bh
				.ENDIF
				AND EBP,3
				MOV EBP,DWORD PTR DS:[ESI+EBP*4]
				ADD EBP,EAX
				XOR EBX,EBP
				.IF BYTE PTR [esp+4*SIZEOF DWORD]!=0
					ADD ECX,EBX
					SUB EAX,61C88647h
				.ELSEIF BYTE PTR [esp+4*SIZEOF DWORD]==0
					SUB	ECX,EBX
					ADD	EAX,61C88647h
				.ENDIF
				MOV EBX,ECX
				SHR EBX,5
				MOV EBP,ECX
				SHL EBP,4
				XOR EBX,EBP
				ADD EBX,ECX
				MOV EBP,EAX
				.IF BYTE PTR [esp+4*SIZEOF DWORD]!=0
					SHR EBP,0Bh
				.ENDIF
				AND EBP,3
				MOV EBP,DWORD PTR DS:[ESI+EBP*4]
				ADD EBP,EAX
				XOR EBX,EBP
				.IF BYTE PTR [esp+4*SIZEOF DWORD]!=0
					ADD EDX,EBX
				.ELSEIF BYTE PTR [esp+4*SIZEOF DWORD]==0
					SUB	EDX,EBX
				.ENDIF
				DEC EDI
			.UNTIL edi==0
			pop edi
			pop esi
			pop ebp
			pop ebx
			mov eax,XTEAPacketPointer
			.IF BYTE PTR [esp]!=0
				mov DWORD PTR [eax],ecx
				mov DWORD PTR [eax+4],edx
			.ELSEIF BYTE PTR [esp]==0
				mov DWORD PTR [eax],edx
				mov DWORD PTR [eax+4],ecx
			.ENDIF
			add XTEAPacketPointer,8
			add PacketPointer,8
			dec NumberOfBlocks
		.UNTIL NumberOfBlocks==0
		
		add esp,4
		
		;Return the length of the packet
		mov ecx,XTEAPacket
		movzx eax,WORD PTR [ecx]
		add eax,2
		
	.ENDIF
	
	ret
XTEA endp

End