SECTIONS 
{
        . = 0x00000000;
        .text : {   
                    head.o (.text);
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
}
