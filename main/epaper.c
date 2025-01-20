#include "epaper.h"
#include <inttypes.h>


static const char *TAG = "SSD1680";
static spi_device_handle_t spi;



int bw_bufsize;         // size of the black and white buffer
int red_bufsize;        // size of the red buffer

uint8_t *bw_buf;        // the pointer to the black and white buffer if using on-chip ram
uint8_t *red_buf;       // the pointer to the red buffer if using on-chip ram


uint8_t debugB = 0;

static void spi_cmd(const uint8_t cmd) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &cmd;
    t.user = (void*)0;
    gpio_set_level(PIN_DC, 0);
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

static void spi_data(const uint8_t data) {
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = 8;
    t.tx_buffer = &data;
    t.user = (void*)1;
    gpio_set_level(PIN_DC, 1);
    ret = spi_device_polling_transmit(spi, &t);
    assert(ret == ESP_OK);
}

static void wait_busy(void) {
    while(gpio_get_level(PIN_BUSY) == 1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void epaper_init(void){

    // GPIO configuration
    gpio_set_direction(PIN_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_BUSY, GPIO_MODE_INPUT);
    gpio_set_pull_mode(PIN_BUSY, GPIO_PULLUP_ONLY);
    
    // SPI configuration
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (DISPLAY_WIDTH * DISPLAY_HEIGHT / 8) + 8
    };
    
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4*1000*1000,
        .mode = 0,
        .spics_io_num = PIN_CS,
        .queue_size = 7,
    };
    
    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &buscfg, 0));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &devcfg, &spi));

    bw_buf = (uint8_t *)malloc(DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8 + 1) * 8 / 8);
    red_buf = (uint8_t *)malloc(DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8 + 1) * 8 / 8);
    bw_bufsize = DISPLAY_WIDTH * (DISPLAY_HEIGHT / 8 + 1) * 8 / 8;
    red_bufsize = bw_bufsize;
    ESP_LOGI(TAG, "Buffer sizes - BW: %d, Red: %d", bw_bufsize, red_bufsize);  // Added logging
    memset(bw_buf, 0xFF, bw_bufsize);
    //memset(bw_buf, 0x00, bw_bufsize);
    memset(red_buf, 0x00, red_bufsize);


    // Reset ekran
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    wait_busy();

    spi_cmd(0x12);  // Software reset
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_busy();


    spi_cmd(0x01); //Driver output control
    spi_data(0xF9);
    spi_data(0x00);
    spi_data(0x00);
    
    
    spi_cmd(0x11); //data entry mode
    spi_data(0x01);

    spi_cmd(0x44); //set Ram-X address start/end position
    spi_data(0x00);
    spi_data(0x0F); //0x0F-->(15+1)*8=128

    spi_cmd(0x45);  //set Ram-Y address start/end position
    spi_data(0xF9); //0xF9-->(249+1)=250
    spi_data(0x00);
    spi_data(0x00);
    spi_data(0x00);
    


    spi_cmd(0x3C); //BorderWavefrom
    spi_data(0x05);

    spi_cmd(0x18); //Read built-in temperature sensor
    spi_data(0x80);

    spi_cmd(0x21); //  Display update control
    spi_data(0x00);
    spi_data(0x80);

    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);
    wait_busy();
    
}

void epaper_reset(){
    

    // Reset ekran
    gpio_set_level(PIN_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    
    wait_busy();

    spi_cmd(0x12);  // Software reset
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_busy();


    spi_cmd(0x01); //Driver output control
    spi_data(0xF9);
    spi_data(0x00);
    spi_data(0x00);
    
    
    spi_cmd(0x11); //data entry mode
    spi_data(0x01);

    spi_cmd(0x44); //set Ram-X address start/end position
    spi_data(0x00);
    spi_data(0x0F); //0x0F-->(15+1)*8=128

    spi_cmd(0x45);  //set Ram-Y address start/end position
    spi_data(0xF9); //0xF9-->(249+1)=250
    spi_data(0x00);
    spi_data(0x00);
    spi_data(0x00);
    


    spi_cmd(0x3C); //BorderWavefrom
    spi_data(0x05);

    spi_cmd(0x18); //Read built-in temperature sensor
    spi_data(0x80);

    spi_cmd(0x21); //  Display update control
    spi_data(0x00);
    spi_data(0x80);

    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);
    wait_busy();
    ESP_LOGI(TAG, "Epaper Display Reset");
}

void epaper_update(void){
    spi_cmd(0x22);
    spi_data(0xF7);
    spi_cmd(0x20);
    vTaskDelay(pdMS_TO_TICKS(100));
    wait_busy();
    ESP_LOGI(TAG, "Epaper Display Updated");
}
void epaper_clear(void){
    memset(bw_buf, 0xFF, bw_bufsize);
    memset(red_buf, 0x00, red_bufsize);
    
    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);
    wait_busy();


    spi_cmd(0x24); //write RAM for black(0)/white (1)

    for (uint16_t i = 0; i < bw_bufsize; i++)
    {
        spi_data(bw_buf[i]);
    }

    spi_cmd(0x26); //write RAM for red(1)/white (0)

    for (uint16_t i = 0; i < red_bufsize; i++)
    {
        spi_data(red_buf[i]);
    }

    epaper_update();
    
}


void epaper_writeBufferToDisplay(void){
    
    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);
    wait_busy();


    spi_cmd(0x24); //write RAM for black(0)/white (1)

    for (uint16_t i = 0; i < bw_bufsize; i++)
    {
        spi_data(bw_buf[i]);
    }

    spi_cmd(0x26); //write RAM for red(1)/white (0)

    for (uint16_t i = 0; i < red_bufsize; i++)
    {
        spi_data(red_buf[i]);
    }
    
}


void epaper_deep_sleep(void){
    spi_cmd(0x10);
    spi_data(0x01); // Deep sleep mode 1
    ESP_LOGI(TAG, "Epaper Display Deep Sleep");
}
void epaper_draw_blackBitmap(const unsigned char IMAGE[]){
    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);

    spi_cmd(0x24); //write RAM for black(0)/white (1)

    for (uint16_t i = 0; i < bw_bufsize; i++)
    {
        spi_data(~IMAGE[i]);
    }
}
void epaper_draw_redBitmap(const unsigned char IMAGE[]){
    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);

    spi_cmd(0x26); //write RAM for red(1)/white (0)

    for (uint16_t i = 0; i < red_bufsize; i++)
    {
        spi_data(IMAGE[i]);
    }
}


void epaper_draw_blackAndRedBitmaps(const unsigned char IMAGE_BLACK[], const unsigned char IMAGE_RED[]){
    
    spi_cmd(0x4E); // set RAM x address count to 0;
    spi_data(0x00);
    spi_cmd(0x4F); // set RAM y address count to 0X199;
    spi_data(0xF9);
    spi_data(0x00);
    wait_busy();


    spi_cmd(0x24); //write RAM for black(0)/white (1)

    for (uint16_t i = 0; i < bw_bufsize; i++)
    {
        spi_data(~IMAGE_BLACK[i]);
    }

    spi_cmd(0x26); //write RAM for red(1)/white (0)

    for (uint16_t i = 0; i < red_bufsize; i++)
    {
        spi_data(IMAGE_RED[i]);
    }
}


void epaper_wait_seconds(uint32_t seconds) {
    vTaskDelay(pdMS_TO_TICKS(seconds * 1000));
    ESP_LOGI(TAG, "Waited %d seconds", (int)seconds);
}

void epaper_wait_milliseconds(uint32_t milliseconds) {
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
    ESP_LOGI(TAG, "Waited %d miliseconds", (int)milliseconds);
}

void epaper_bw_buffer_clear(void){
    memset(bw_buf, 0xFF, bw_bufsize);
}
void epaper_red_buffer_clear(void){
    
    memset(red_buf, 0x00, red_bufsize);
}



void epaper_draw_partial_blackAndRedBitmapsEnhanced(const unsigned char* black_bitmap, const unsigned char* red_bitmap, int x, int y, int width, int height, int bitmap_width, int bitmap_height) {
// control size of Image buffer's borders
    if (x < 0 || y < 0 || x + width > DISPLAY_WIDTH || y + height > DISPLAY_HEIGHT) {
        return; 
    }
    //int i = 0;
    
    for (int w = 0; w < width; w++) { // lines of Image
        for (int h = 0; h < height; h++) { // pillars of Image
            uint8_t pixel = (black_bitmap[w * 8 + h / 8] >> (7 - (h % 8))) & 0x01;
            uint8_t pixelR = (red_bitmap[w * 8 + h / 8] >> (7 - (h % 8))) & 0x01;
            

            // The corresponding byte and bit position in the buffer
            int buffer_col = x + w; // Column in Buffer
            int buffer_row = (y + h) / 8; // Byte line in Buffer
            int buffer_bit = (y + h) % 8; // Bit position in Buffer

            // Get the corresponding byte in the buffer
            uint8_t *buffer_byte = &bw_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];
            uint8_t *buffer_byteR = &red_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];


            // Place pixel in buffer
            if (!pixel) {
                *buffer_byte |= (1 << (7-buffer_bit)); //Set bit to 1
            } else {
                *buffer_byte &= ~(1 << (7-buffer_bit)); // Set bit to 0
            }

            if (pixelR) {
                *buffer_byteR |= (1 << (7-buffer_bit)); // Set bit to 1
            } else {
                *buffer_byteR &= ~(1 << (7-buffer_bit)); // Set bit to 0
            }
        }
    }
}



void epaper_draw_partial_blackBitmapsEnhanced(const unsigned char* black_bitmap, int x, int y, int width, int height, int bitmap_width, int bitmap_height) {
// control size of Image buffer's borders
    if (x < 0 || y < 0 || x + width > DISPLAY_WIDTH || y + height > DISPLAY_HEIGHT) {
        return; 
    }
    //int i = 0;
    
    for (int w = 0; w < width; w++) { // lines of Image
        for (int h = 0; h < height; h++) { // pillars of Image
            uint8_t pixel = (black_bitmap[w * 8 + h / 8] >> (7 - (h % 8))) & 0x01;
            

            // The corresponding byte and bit position in the buffer
            int buffer_col = x + w; // Column in Buffer
            int buffer_row = (y + h) / 8; // Byte line in Buffer
            int buffer_bit = (y + h) % 8; // Bit position in Buffer

            // Get the corresponding byte in the buffer
            uint8_t *buffer_byte = &bw_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];


            // Place pixel in buffer
            if (!pixel) {
                *buffer_byte |= (1 << (7-buffer_bit)); //Set bit to 1
            } else {
                *buffer_byte &= ~(1 << (7-buffer_bit)); // Set bit to 0
            }
        }
    }
}



void place_image_into_buffer(int x, int y,const unsigned char* black_bitmap, int IMAGE_WIDTH, int IMAGE_HEIGHT) {
// control size of Image buffer's borders
    if (x < 0 || y < 0 || x + IMAGE_WIDTH > DISPLAY_WIDTH || y + IMAGE_HEIGHT > DISPLAY_HEIGHT) {
        return;
    }


    for (int i = 0; i < IMAGE_HEIGHT; i++) { 
        for (int j = 0; j < IMAGE_WIDTH; j++) { 

            uint8_t pixel = (black_bitmap[i * (IMAGE_WIDTH / 8) + j / 8] >> (7 - (j % 8))) & 0x01;

            int buffer_col = x + j; 
            int buffer_row = (y + i) / 8; 
            int buffer_bit = (y + i) % 8; 


            uint8_t *buffer_byte = &bw_buf[buffer_row * DISPLAY_WIDTH + buffer_col];


            if (pixel) {
                *buffer_byte |= (1 << buffer_bit); 
            } else {
                *buffer_byte &= ~(1 << buffer_bit); 
            }
        }
    }
}


void draw_pixel(int w, int h, uint8_t pixel, int x, int y){

    int buffer_col = x + w; 
    int buffer_row = ( y + h ) / 8; 
    int buffer_bit = ( y + h ) % 8; 
    uint8_t *buffer_byte = &bw_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];
    if (!pixel) {
        *buffer_byte |= (1 << (7-buffer_bit));
    } else {
        *buffer_byte &= ~(1 << (7-buffer_bit));
    }
}

void draw_line_horizontal(int x1, int x2, int y){
    for(int i = x1; i < x2; i++){
        draw_pixel(i, y, 1, 0, 0);
    }

}

void draw_line_vertical(int x ,int y1, int y2){
    for(int i = y1; i < y2; i++){
        draw_pixel(x, i, 1, 0, 0);
    }

}

void draw_pixel_color(int w, int h, uint8_t pixel, int x, int y, int color){
    
    int buffer_col = x + w; 
    int buffer_row = ( y + h ) / 8; 
    int buffer_bit = ( y + h ) % 8; 
    uint8_t *buffer_byte = NULL;
    if(color == BLACK){
        buffer_byte = &bw_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];
    }
    else if(color == RED){
        buffer_byte = &red_buf[(buffer_col * (DISPLAY_HEIGHT / 8+1)) + buffer_row];
    }
    if (((!pixel) && (color == BLACK)) || ((pixel) && (color == RED))) {
        *buffer_byte |= (1 << (7-buffer_bit));
    } 
    else if(((pixel) && (color ==BLACK) )|| ((!pixel) && (color == RED))){
        *buffer_byte &= ~(1 << (7-buffer_bit)); 
    }
}


int counter2 = 0;
int counterHeight2 = 0;
int counterByte2 = 0;
int wC = 0;
int hC = 0;


void printBinary2_color(unsigned char value, int length, int height, int x, int y, int color){
    for (int i = 7; i >= 0; i--) {
        if(counterHeight2==height){
            continue;
        }
        
        if(wC==length){
            wC = 0;
            hC++;
        }
        draw_pixel_color(wC, hC, (value >> i) & 1, x, y, color);
        wC++;

        if(debugB){
            if((value >> i) & 1){
                printf("█ ");
            }else{
                printf("  ");
            }
        }
        counter2++;
        if(counter2==length){
            if(debugB)
                printf("\n");
            counterHeight2++;
            counter2 = 0;
        }
    }
}



void draw_char_withFont_color(uint8_t letter, int x, int y, const GFXfont *font, uint8_t color){
    int widthOfLetter = font->glyph[letter - font->first].width;
    int heightOfLetter = font->glyph[letter - font->first].height;
    if(debugB)
        ESP_LOGI(TAG, "Char %c: \nBitmapOffset: %d \nWidth: %d \nHeight: %d \nxAdvance: %d \nxOffset: %d \nyOffset: %d \nyAdvance: %d \nFirstByte: %.4x", 
             letter,
             font->glyph[letter - font->first].bitmapOffset,
             widthOfLetter,
             heightOfLetter,
             font->glyph[letter - font->first].xAdvance,
             font->glyph[letter - font->first].xOffset,
             font->glyph[letter - font->first].yOffset,
             font->yAdvance,
             font->bitmap[font->glyph[letter - font->first].bitmapOffset]);

    for(int i = 0; i < (widthOfLetter * heightOfLetter / 8 + 1); i++) {
        printBinary2_color(font->bitmap[font->glyph[letter - font->first].bitmapOffset + i], widthOfLetter, heightOfLetter, x, y, color);
    }
    counter2 = 0;
    counterHeight2 = 0;
    counterByte2 = 0;
    wC = 0;
    hC = 0;
}

void draw_word_withFont_color(const char* word, int x, int y, const GFXfont *font, uint8_t color){  //to do : continue from here
    uint8_t letter = word[0];
    if(debugB)
        ESP_LOGI(TAG, "Word: '%s'", word);
    
    for(int i = 0; i < strlen(word); i++){
            if(debugB)
                ESP_LOGI(TAG, "XHarf: '%c' hex:%02x last:%02x test:%d kontrol:%d", word[i], word[i], font->last , (3<2), word[i] > font->last);

        if(word[i] - font->first < 0 || word[i] > font->last){
            ESP_LOGI(TAG, "YHarf: '%c' hex:%02x skipped", word[i], word[i]);
            continue;
        }
        int yYeni = y + font->glyph[word[i] - font->first].yOffset;
        draw_char_withFont_color(word[i], x, yYeni, font, color);
        x += font->glyph[word[i] - font->first].xAdvance;
        if(debugB)
           ESP_LOGI(TAG, "ZHarf: '%c' hex:%02x Y ekseni: %d Satır Yüksekliği: %d Y Offseti: %d Harf Yüksekliği: %d yYeni: %d",
                        word[i],    word[i],
                                        y,
                                                    font->yAdvance,
                                                                    font->glyph[word[i] - font->first].yOffset,
                                                                                        font->glyph[word[i] - font->first].height,
                                                                                                        yYeni);    
    }
}