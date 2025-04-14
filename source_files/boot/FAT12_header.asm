
; Also known as the bootsector.
; The jump instruction at the beginning is naturally intended to jump past the
; header, so name whatever comes next with the start label!

; ============================== FAT 12 HEADER ================================
jmp 	short start
nop

fat_oem:		db	'MSWIN4.1'
fat_bytes_per_sector:	dw	512	; Bytes per sector
fat_sect_per_cluster:	db	1	; Sectors per cluster
fat_reserved_sectors:	dw	1	; Reserved sectors
fat_fat_count:		db	2	; Number of FATs
fat_dir_entry_count:	dw	0xE0	; Dir entries count
fat_sector_count:	dw	2880	; Sector count
fat_media_type:		db	0xF0	; Media descriptor (disk type, this is 3.5")
fat_sectors_per_fat:	dw	9	; Number of sectors per FAT
fat_sectors_per_track:	dw	18	; Number of sectors per track
fat_head_count:		dw	2	; Head count
fat_hidden_sectors:	dd	0	; Hidden sector count
fat_large_sectors:	dd	0	; Large sector count

fat_drive_num:		db	0	; Drive number
			db	0	; Reserved byte
fat_signature:		db	0x29	; Signature
fat_volume_id:		db	0x42, 0x42, 0x42, 0x42 ; Volume id. Anything goes here.
fat_volume_label:	db	'DOROS      '	; Volume label, 11 byte str with space char padding
fat_system_id		db	'FAT12   '	; Same thing but FAT12 and 8 bytes

; - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
