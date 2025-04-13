#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>


// FAT12 Header aka. bootsector
typedef struct
{
	uint8_t		shortjump[3];

	uint8_t		fat_oem[8];
	uint16_t	fat_bytes_per_sector;
	uint8_t		fat_sect_per_cluster;
	uint16_t	fat_reserved_sector;
	uint8_t		fat_fat_count;
	uint16_t	fat_dir_entry_count;
	uint16_t	fat_sector_count;
	uint8_t		fat_media_type;
	uint16_t	fat_sectors_per_fat;
	uint16_t	fat_sectors_per_track;
	uint16_t	fat_head_count;
	uint32_t	fat_hidden_sectors;
	uint32_t	fat_large_sectors;

	uint8_t		fat_drive_num;
	uint8_t		reservedbyte;
	uint8_t		fat_signature;
	uint32_t	fat_volume_id;
	uint8_t		fat_volume_label[11];
	uint8_t		fat_system_id[8];

	// This attribute tells the compiler not to pad the struct
}	__attribute__((packed))	fatheader;

// Directory entry
typedef struct
{
	uint8_t		name[11];
	uint8_t		attributes;
	uint8_t		reservedbyte;
	uint8_t		creation_time_tenthofseconds;
	uint16_t	creation_time;
	uint16_t	creation_date;
	uint16_t	access_date;
	uint16_t	first_cluster_high;
	uint16_t	modify_time;
	uint16_t	modify_date;
	uint16_t	first_cluster_low;
	uint32_t	file_size;
	// This attribute tells the compiler not to pad the struct
}	__attribute__((packed))	fat_entry;

// NOTE this is an unnecessary use of global variables.
// It is merely to make this faster to write, just a test code file!
fatheader	g_fatheader;
uint8_t		*g_fat = NULL;
fat_entry		*g_fatrootdir;
uint32_t	g_fatrootdir_end;

// Read FAT12 header
bool readHeader(FILE *disk)
{
	return (fread(&g_fatheader, sizeof(g_fatheader), 1, disk) > 0);
}

// Read N sectors from LBA
bool readSectors(FILE *disk, uint32_t lba, uint32_t sect_count, void *buffer)
{
	if (fseek(disk, lba * g_fatheader.fat_bytes_per_sector, SEEK_SET) != 0)
		return false;

	if (fread(buffer, g_fatheader.fat_bytes_per_sector, sect_count, disk) != sect_count)
		return false;

	return true;
}

// Read FAT table
bool readFAT(FILE *disk)
{
	g_fat = malloc(g_fatheader.fat_sectors_per_fat * g_fatheader.fat_bytes_per_sector);
	// Not checking malloc for null we dont care for test code
	return readSectors(disk, g_fatheader.fat_reserved_sector, g_fatheader.fat_sectors_per_fat, g_fat);
}

bool readRootdir(FILE *disk)
{
	// LBA address of directory
	uint32_t lba = g_fatheader.fat_reserved_sector + g_fatheader.fat_fat_count * g_fatheader.fat_sectors_per_fat;

	// Size of directory in bytes
	uint32_t size = sizeof(fat_entry) * g_fatheader.fat_dir_entry_count;

	// How many sectors to read. Size of dir / size of sector
	uint32_t sector_count = (size / g_fatheader.fat_bytes_per_sector);

	// Round division up
	if (size % g_fatheader.fat_bytes_per_sector != 0)
		++sector_count;

	// Again do not care to check for null just a proof of concept code
	g_fatrootdir = malloc(sector_count * g_fatheader.fat_bytes_per_sector);
	g_fatrootdir_end = sector_count + lba;
	return readSectors(disk, lba, sector_count, g_fatrootdir);
}

fat_entry *findFile(const char *name)
{
	for (uint32_t i = 0; i < g_fatheader.fat_dir_entry_count; ++i)
	{
		if (memcmp(g_fatrootdir[i].name, name, 11) == 0)
			return (&(g_fatrootdir[i]));
	}

	return 0;
}

bool readFile(fat_entry *file_entry, FILE *disk, uint8_t *buffer)
{
	uint16_t	currentCluster = file_entry->first_cluster_low;

	uint32_t	fatIndex;
	uint32_t	lba; // Data segment location + current cluster
	while (currentCluster < 0x0FF8) // End of chain marker
	{
		// Clusters start from 2 for some reason on FAT. So just subtract 2
		lba = g_fatrootdir_end + (currentCluster - 2) * g_fatheader.fat_sect_per_cluster;
		if (!readSectors(disk, lba, g_fatheader.fat_sect_per_cluster, buffer))
			return false;
		buffer += g_fatheader.fat_sect_per_cluster * g_fatheader.fat_bytes_per_sector;

		// Fat 12 being 12 bits makes this kind of disgusting... It's just getting the 12 bits
		fatIndex = currentCluster * 3 / 2;
		if (currentCluster % 2 == 0) // even number
			currentCluster = (*(uint16_t*)(g_fat + fatIndex)) & 0x0FFF;
		else
			currentCluster = (*(uint16_t*)(g_fat + fatIndex)) >> 4;
	}
	return true;
}


// Very fastly written proof of concept program for reading FAT12 directories
// Argument 1 = disk name
// Argument 2 = file name
int	main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("%s\n", "Bad argc");
		return 1;
	}

	// Read in binary mode
	FILE *disk = fopen(argv[1], "rb");
	if (!disk)
		{ printf("%s\n", "Opening disk failed"); return 2; }

	if (!readHeader(disk))
		{ printf("%s\n", "Reading header failed"); return 3; }

	if (!readFAT(disk))
		{ printf("%s\n", "Reading FAT failed"); return 4; }
	// ^ Should free memory here but we do not care. Just a test code

	if (!readRootdir(disk))
		{ printf("%s\n", "Reading rootdir failed"); return 5; }
	// ^ Should free memory here but we do not care. Just a test code

	fat_entry	*file_entry = findFile(argv[2]);
	if (file_entry == NULL)
		{ printf("%s\n", "finding file failed"); return 6; }
	// ^ Should free memory here but we do not care. Just a test code

	uint8_t *buffer = malloc(file_entry->file_size + g_fatheader.fat_bytes_per_sector);
	if (!readFile(file_entry, disk, buffer))
		{ printf("%s\n", "reading file failed"); return 7; }
	// ^ Should free memory here but we do not care. Just a test code
	printf("%s\n", buffer);


	free (g_fat);
	free (g_fatrootdir);
	free (buffer);
	return 0;
}
