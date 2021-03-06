#pragma once

typedef float fptp_t;
typedef uint8_t uc_t;

typedef enum
{
    DL_C_IMPL = 0,
    DL_XTENSA_IMPL = 1
} dl_conv_mode;

typedef enum
{
    INPUT_UINT8 = 0,
    INPUT_FLOAT = 1,
} dl_op_type;

typedef enum
{
    PADDING_VALID = 0,
    PADDING_SAME = 1,
} dl_padding_type;

/*
 * Matrix for 3d
 * @Warning: the sequence of variables is fixed, cannot be modified, otherwise there will be errors in esp_dsp_dot_float
 */
typedef struct
{
    /******* fix start *******/
    int w;  // Width
    int h;  // Height
    int c;  // Channel
    int n;  // Number, to record filter's out_channels. input and output must be 1
    int stride;
    fptp_t *item;
    /******* fix end *******/
} dl_matrix3d_t;

typedef struct
{
    int w;  // Width
    int h;  // Height
    int c;  // Channel
    int n;  // Number, to record filter's out_channels. input and output must be 1
    int stride;
    uc_t *item;
} dl_matrix3du_t;


typedef struct
{
    int stride_x;
    int stride_y;
    dl_padding_type padding;
    dl_conv_mode mode;
    dl_op_type type;
} dl_matrix3d_conv_config_t;

/*
 * @brief Allocate a 3D matrix with float items, the access sequence is NHWC
 *
 * @param n     Number of matrix3d, for filters it is out channels, for others it is 1
 * @param w     Width of matrix3d
 * @param h     Height of matrix3d
 * @param c     Channel of matrix3d
 * @return      3d matrix
 */
dl_matrix3d_t *dl_matrix3d_alloc(int n, int w, int h, int c);

/*
 * @brief Allocate a 3D matrix with 8-bits items, the access sequence is NHWC
 *
 * @param n     Number of matrix3d, for filters it is out channels, for others it is 1
 * @param w     Width of matrix3d
 * @param h     Height of matrix3d
 * @param c     Channel of matrix3d
 * @return      3d matrix
 */
dl_matrix3du_t *dl_matrix3du_alloc(int n, int w, int h, int c);

/*
 * @brief Free a matrix3d
 *
 * @param m matrix3d with float items
 */
void dl_matrix3d_free(dl_matrix3d_t *m);

/*
 * @brief Free a matrix3d
 *
 * @param m matrix3d with 8-bits items
 */
void dl_matrix3du_free(dl_matrix3du_t *m);

/**
 * @brief Do a relu (Rectifier Linear Unit) operation, update the input matrix3d
 *
 * @param in        Floating point input matrix3d
 * @param clip      If value is higher than this, it will be clipped to this value
 */
void dl_matrix3d_relu (dl_matrix3d_t *m, fptp_t clip);

/**
 * @brief Do a leaky relu (Rectifier Linear Unit) operation, update the input matrix3d
 *
 * @param in        Floating point input matrix3d
 * @param clip      If value is higher than this, it will be clipped to this value
 * @param alpha     If value is less than zero, it will be updated by multiplying this factor
 */
void dl_matrix3d_leaky_relu (dl_matrix3d_t *m, fptp_t clip, fptp_t alpha);

/**
 * @brief Do a softmax operation on a matrix3d
 *
 * @param in        Input matrix3d
 */
void dl_matrix3d_softmax (dl_matrix3d_t *m);


/**
 * @brief Do a general fully connected layer pass, dimension is (number, width, height, channel)
 *
 * @param in             Input matrix3d, size is (1, w, 1, 1)
 * @param filter         Weights of the neurons, size is (1, w, h, 1)
 * @param bias           Bias for the fc layer, size is (1, 1, 1, h)
 * @return               The result of fc layer, size is (1, 1, 1, h)
 */
dl_matrix3d_t *dl_matrix3d_fc (dl_matrix3d_t *in, dl_matrix3d_t *filter, dl_matrix3d_t *bias);


/**
 * @brief Copy a range of float items from an existing matrix to a preallocated matrix
 *
 * @param dst   The destination slice matrix
 * @param src   The source matrix to slice
 * @param x     X-offset of the origin of the returned matrix within the sliced matrix
 * @param y     Y-offset of the origin of the returned matrix within the sliced matrix
 * @param w     Width of the resulting matrix
 * @param h     Height of the resulting matrix
 */
void dl_matrix3d_slice_copy (dl_matrix3d_t *dst, dl_matrix3d_t *src, int x, int y, int w, int h);

/**
 * @brief Copy a range of 8-bits items from an existing matrix to a preallocated matrix
 *
 * @param dst   The destination slice matrix
 * @param src   The source matrix to slice
 * @param x     X-offset of the origin of the returned matrix within the sliced matrix
 * @param y     Y-offset of the origin of the returned matrix within the sliced matrix
 * @param w     Width of the resulting matrix
 * @param h     Height of the resulting matrix
 */
void dl_matrix3du_slice_copy (dl_matrix3du_t *dst, dl_matrix3du_t *src, int x, int y, int w, int h);


/**
 * @brief Do a general CNN layer pass, dimension is (number, width, height, channel)
 *
 * @param in             Input matrix3d
 * @param filter         Weights of the neurons
 * @param bias           Bias for the CNN layer
 * @param stride_x       The step length of the convolution window in x(width) direction
 * @param stride_y       The step length of the convolution window in y(height) direction
 * @param padding        One of VALID or SAME
 * @param mode           Do convolution using C implement or xtensa implement, 0 or 1, with respect
 *                       If ESP_PLATFORM is not defined, this value is not used. Default is 0
 * @return               The result of CNN layer
 */
dl_matrix3d_t *dl_matrix3d_conv (dl_matrix3d_t *in, dl_matrix3d_t *filter, dl_matrix3d_t *bias,
                                    int stride_x, int stride_y, int padding, int mode);

/**
 * @brief Do a general CNN layer pass, dimension is (number, width, height, channel)
 *
 * @param in             Input matrix3d
 * @param filter         Weights of the neurons
 * @param bias           Bias for the CNN layer
 * @param stride_x       The step length of the convolution window in x(width) direction
 * @param stride_y       The step length of the convolution window in y(height) direction
 * @param padding        One of VALID or SAME
 * @param mode           Do convolution using C implement or xtensa implement, 0 or 1, with respect
 *                       If ESP_PLATFORM is not defined, this value is not used. Default is 0
 * @return               The result of CNN layer
 */
dl_matrix3d_t *dl_matrix3du_conv (dl_matrix3du_t *in, dl_matrix3d_t *filter, dl_matrix3d_t *bias,
                                    int stride_x, int stride_y, int padding, int mode);


/**
 * @brief Do a depthwise CNN layer pass, dimension is (number, width, height, channel)
 *
 * @param in             Input matrix3d
 * @param filter         Weights of the neurons
 * @param stride_x       The step length of the convolution window in x(width) direction
 * @param stride_y       The step length of the convolution window in y(height) direction
 * @param padding        One of VALID or SAME
 * @param mode           Do convolution using C implement or xtensa implement, 0 or 1, with respect
 *                       If ESP_PLATFORM is not defined, this value is not used. Default is 0
 * @return               The result of depthwise CNN layer
 */
dl_matrix3d_t *dl_matrix3d_depthwise_conv (dl_matrix3d_t *in, dl_matrix3d_t *filter,
                                    int stride_x, int stride_y, int padding, int mode);

/**
 * @brief Do a mobilenet block forward, dimension is (number, width, height, channel)
 *
 * @param in             Input matrix3d
 * @param filter         Weights of the neurons
 * @param stride_x       The step length of the convolution window in x(width) direction
 * @param stride_y       The step length of the convolution window in y(height) direction
 * @param padding        One of VALID or SAME
 * @param mode           Do convolution using C implement or xtensa implement, 0 or 1, with respect
 *                       If ESP_PLATFORM is not defined, this value is not used. Default is 0
 * @return               The result of depthwise CNN layer
 */
dl_matrix3d_t *dl_matrix3d_mobilenet (void *in, dl_matrix3d_t *dilate, dl_matrix3d_t *depthwise,
                                        dl_matrix3d_t *compress, dl_matrix3d_t *bias, dl_matrix3d_t *prelu,
                                        dl_matrix3d_conv_config_t *config);


/**
 * @brief Print the matrix3d items
 *
 * @param m              dl_matrix3d_t to be printed
 * @param message        name of matrix
 */
void dl_matrix3d_print (dl_matrix3d_t *m, char *message);

/**
 * @brief Print the matrix3du items
 *
 * @param m              dl_matrix3du_t to be printed
 * @param message        name of matrix
 */
void dl_matrix3du_print (dl_matrix3du_t *m, char *message);
