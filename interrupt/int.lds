SECTIONS 
{
        . = 0x00000000;
        .text : {   
                    * (vectors);
                    * (.text);
                }
        flash_sdata = .;
        
        . = 0x30000000;
        ram_sdata = .;
        .data : AT(flash_sdata) 
                {
                    * (.data);
                }
        ram_edata = .;
        data_size = ram_edata - ram_sdata;
        
        bss_s = .;
        .bss : {
                * (.bss);
            }
        bss_e = .;
        bss_size = bss_e - bss_s;
}
