/**
* @file tensor.cpp
*
* @brief A class representing tensors for complex algebraic manipuplations of
* and things within coordinate frames in 3-dimensional space
*
* @author Pavlo Vlastos
*/

#include "tensor.h"
#include "tensor_config.h"

/******************************************************************************
 * PUBLIC FUNCTION IMPLEMENTATIONS
 *****************************************************************************/
tensor_status tensor::set_tensor_element(const unsigned int row,
                                         const unsigned int col,
                                         double value) {
    tensor_status status = tensor_status::SUCCESS;
    if ((row >= 0) && (row < m_height) && (col >= 0) && (col < n_width)) {
        content[row][col] = value;

    } else {
        status = tensor_status::FAILURE;
    }

    return status;
}

tensor_status tensor::set_tensor_content(const vector<vector<double>>& vv) {
    tensor_status status = tensor_status::SUCCESS;

    if ((vv.size() == m_height) && (vv[0].size() == n_width)) {
        for (unsigned int row = 0; row < m_height; row++) {
            for (unsigned int col = 0; col < n_width; col++) {
                content[row][col] = vv[row][col];
            }
        }
    }

    return status;
}

tensor multiply(const tensor& a, const tensor& b) {
    tensor c(a.m_height, b.n_width);

    /* Check tensor dimensions */
    if (a.n_width == b.m_height) {

        /* Iterate through rows in tensor c */
        for (unsigned int i = 0; i < a.m_height; i++) {

            /* Iterate through columns in tensor b */
            for (unsigned int j = 0; j < b.n_width; j++) {
                c.content[i][j] = 0.0;

                /* Iterate through elements in row of tensor a, and column
                 * of tensor b, respectively */
                for (unsigned int k = 0; k < b.m_height; k++) {
                    c.content[i][j] += (a.content[i][k] * b.content[k][j]);
                }
            }
        }
    }

    return c;
}

tensor copy(const tensor& a) {
    tensor b(a.m_height, a.n_width);

    b.set_tensor_content(a.content);

    return b;
}

tensor transpose(const tensor& a) {
    tensor b(a.n_width, a.m_height);

    for (unsigned int i = 0; i < b.m_height; i++) {
        for (unsigned int j = 0; j < b.n_width; j++) {
            b.content[i][j] = a.content[j][i];
        }
    }

    return b;
}

tensor invert(const tensor& a) {
    tensor a_aug(a.m_height, 2 * a.n_width);

    a_aug.print_tensor();

    return a_aug;
}

//tensor gaussian_elimination(const tensor& a) {
//    tensor b(a.m_height, a.n_width);
//
//    /* Reduce to uppder triangular form */
//    for (int i = 0; i < m_height; i++) {
//        for (int j = 0; j < n_width; j++) {
//
//        }
//    }
//}

tensor augment_width(const tensor& a, const tensor& b) {
    tensor c(a.m_height, a.n_width + b.n_width);

    if (a.m_height == b.m_height) {
        for (unsigned int i = 0; i < c.m_height; i++) {
            for (unsigned int j = 0; j < c.n_width; j++) {
                if (j < a.n_width) {
                    c.content[i][j] = a.content[i][j];
                } else if ((j >= a.n_width) && (j < c.n_width)) {
                    c.content[i][j] = b.content[i][j - a.n_width];
                }
            }
        }
    }

    return c;
}

tensor augment_height(const tensor& a, const tensor& b) {
    tensor c(a.m_height + b.m_height, a.n_width);

    if (a.n_width == b.n_width) {
        for (unsigned int i = 0; i < c.m_height; i++) {
            for (unsigned int j = 0; j < c.n_width; j++) {
                if (i < a.m_height) {
                    c.content[i][j] = a.content[i][j];
                } else if ((i >= a.m_height) && (i < c.m_height)) {
                    c.content[i][j] = b.content[i - a.n_width][j];
                }
            }
        }
    }

    return c;
}

tensor_status eye(tensor& a) {
    tensor_status status = tensor_status::FAILURE;

    if (a.m_height == a.n_width) {
        for (unsigned int i = 0; i < a.m_height; i++) {
            for (unsigned int j = 0; j < a.n_width; j++) {
                if (i == j) {
                    a.content[i][j] = 1.0; // Place ones on the diagonal
                } else {
                    a.content[i][j] = 0.0;
                }
            }
        }
        status = tensor_status::SUCCESS;
    }

    return status;
}

tensor_status tensor::swap_rows(int row_a, int row_b) {
    double temp_element = 0.0;
    tensor_status status = tensor_status::FAILURE;

    if ((row_a >= 0) && (row_b >= 0) && (row_a != row_b)) {
        /* Copy the row to be swapped and begin swapping each element */
        for (unsigned int j = 0; j < n_width; j++) {
            temp_element = content[row_a][j];
            content[row_a][j] = content[row_b][j];
            content[row_b][j] = temp_element;
        }

        status = tensor_status::SUCCESS;
    } else if (row_a == row_b) {
        status = tensor_status::SUCCESS;
    } else {
        status = tensor_status::FAILURE;
    }

    return status;
}

/******************************************************************************
 * UNIT TESTS
 *****************************************************************************/
#ifdef TESTING
#include <iostream>

void tensor::print_tensor(void) {
    for (unsigned int row = 0; row < m_height; row++) {
        cout << "[ ";
        for (unsigned int col = 0; col < n_width; col++) {
            cout << content[row][col] << " ";
        }
        cout << "]\n";
    }
    cout << "Dimensions: " << m_height << " x " << n_width << "\n";
}

using namespace std;

int main(void) {

#ifdef TEST_TENSOR_CLASS_CONSTRUCTOR
    cout << "Testing tensor constructor\r\n";
    int rows;
    int cols;
    rows = 4;
    cols = 3;

    tensor tnsr0(rows, cols);

    tnsr0.print_tensor();

    tensor a(vector<vector<double>> { { 1, 2 }, { 2, 1 }, { 1, 2 },
             { 2, 1 } });
    a.print_tensor();
#endif

#ifdef TEST_TENSOR_MULTIPLICATION
    {
        cout << "Testing tensor multiplication\r\n";
        tensor a(vector<vector<double>> { { 1, 2, 0 }, { 2, 1, 0 } });
        tensor b(vector<vector<double>> { { 1 }, { 2 }, { 3 } });

        cout << "first operand tensor:\r\n";
        a.print_tensor();

        cout << "second operand tensor:\r\n";
        b.print_tensor();

        tensor c = multiply(a, b);

        c.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_COPY
    {
        cout << "Testing tensor copy()\r\n";
        cout << "first operand tensor to be copied:\r\n";
        tensor a(vector<vector<double>> { { 1, 2, 5 }, { 2, 1, 50.02 } });
        tensor b = copy(a);
        a.print_tensor();
        b.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_TRANSPOSE
    {
        cout << "Testing tensor transpose()\r\n";
        cout << "first operand tensor to be tranposed:\r\n";
        tensor a(vector<vector<double>> { { 1.0, 3.2, 0.5 }, { 0.2, 1.0, 50.02 },
                 { 0.1, 11, 25.01 }});
        tensor b = transpose(a);
        a.print_tensor();
        b.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_SWAP_ROWS
    {
        tensor a(vector<vector<double>> { { 1.0, 3.2, 0.5 }, { 0.2, 1.0, 50.02 },
                 { 0.1, 11, 25.01 }});
        a.print_tensor();
        a.swap_rows(0, 2);
        a.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_AUGMENT_WIDTH
    {
        tensor a(vector<vector<double>> { { 1.0, 2.0, 3.0 }, { 0.0, 1.0, 4.0 },
                 { 5.0, 6.0, 1.0 }});
        a.print_tensor();

        tensor b = copy(a);
        b.print_tensor();

        tensor c = augment_width(a, b);
        c.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_AUGMENT_HEIGHT
    {
        tensor a(vector<vector<double>> { { 1.0, 2.0, 3.0 }, { 0.0, 1.0, 4.0 },
                 { 5.0, 6.0, 1.0 }});
        a.print_tensor();

        tensor b = copy(a);
        b.print_tensor();

        tensor c = augment_height(a, b);
        c.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_EYE
    {
        tensor a(vector<vector<double>> { { 1.0, 2.0, 3.0 }, { 0.0, 1.0, 4.0 },
                 { 5.0, 6.0, 1.0 }});
        a.print_tensor();

        eye(a);
        a.print_tensor();
    }
#endif
#ifdef TEST_TENSOR_INVERT
    {
        tensor a(vector<vector<double>> { { 1.0, 2.0, 3.0 }, { 0.0, 1.0, 4.0 },
                 { 5.0, 6.0, 1.0 }});
        a.print_tensor();

        tensor b = invert(a);
        b.print_tensor();
    }
#endif
    return 1;
}
#endif