#include "CurveFitting.hpp"

#include "assert.h"

#include <math.h>
#include <deque>
#include <memory>

using namespace std;

#define vec vector<float>

const float sign(float val){ 
    return (val >= 0) ? 1.0 : -1.0;
}

bool floatIsZero(float a)
{
    return fabs(a) < numeric_limits<float>::epsilon();
}

const float Point::len() const {
    return sqrtf(x*x + y*y);
}

const Point Point::pow(const Point p, int power){
    return Point(powf(p.x, power), powf(p.x, power));
}

void Point::DebugDisplay(const char* name) const{
    printf("Point %s:\n", name);
    printf("\tX: %f,\n", x);
    printf("\tY: %f,\n", y);
    printf("\\Point\n\n");
}

struct Vector
{
    Vector(const vec contents) : contents(contents) {}

    static const Vector CreateBaseFor(const int row, const int height){
        vec result(height, 0);
        result[row] = 1;
        return result;
    }

    const size_t Size() const{
        return contents.size();
    }

    void DebugDisplay(const char* name) const{
        printf("Vector %s:\n", name);
        for (size_t i = 0; i < Size(); i++)
        {
            printf("\t%f,\n", Get(i));
        }
        printf("\\Vector\n\n");
    }

    const float Get(const size_t index) const{
        assert(index >= 0, "Index cannot be negative");
        assert(index < contents.size(), "Index must be whitin range!");

        return contents[index];
    }

    const Vector Subsection(const int start, int count) const{
        if (count < 0) count = Size() - start;
        
        assert(start >= 0, "Start must not be negative"); 
        assert(start < (int)Size(), "Start must exceed length"); 

        assert(start + count <= (int)Size(), "Subsection must not exceed vector.");

        if(count == 0) return vec();

        vec result; result.reserve(count);
        for (int i = 0; i < count; i++)
        {
            result.push_back(contents[start+i]);
        }
        
        return result;
    }

    const Vector Reverse() const{
        vec result; result.reserve(contents.size());
        copy(contents.crbegin(), contents.crend(), back_inserter(result));
        return result;
    }

    const Vector operator+(const Vector rhs) const{
        assert(Size() == rhs.Size(), "Mismatched vectors are not allowed.");
        vec result(Size(), 0);
        for (size_t i = 0; i < Size(); i++)
        {
            result[i] = Get(i) + rhs.Get(i);
        }
        return result;
    }

    const Vector operator-(const Vector rhs) const{
        assert(Size() == rhs.Size(), "Mismatched vectors are not allowed.");
        vec result(Size(), 0);
        for (size_t i = 0; i < Size(); i++)
        {
            result[i] = Get(i) - rhs.Get(i);
        }
        return result;
    }

    const Vector operator*(const float rhs) const{
        vec result(Size(), 0);
        for (size_t i = 0; i < Size(); i++)
        {
            result[i] = Get(i) * rhs;
        }
        return result;
    }

    const Vector operator/(const float rhs) const{
        vec result(Size(), 0);
        float x = 1 / rhs; // Lessen the number of divisions
        for (size_t i = 0; i < Size(); i++)
        {
            result[i] = Get(i) * x;
        }
        return result;
    }

    const float Magnitude() const{
        float square_sum = 0;
        for (size_t i = 0; i < Size(); i++)
        {
            square_sum += Get(i)*Get(i);
        }
        return sqrtf(square_sum);
    }

    const Vector Normalize() const {
        if (Magnitude() == 0) return *this;
        return *this / Magnitude();
    }

    explicit operator const vec() const { return contents; }

    private:
    const vec contents;
};

const Vector operator*(float lhs, const Vector rhs){
    return rhs * lhs;
}

struct Matrix
{
    private:
    const vec contents;

    public:
    const int m, n;

    Matrix(int m, int n, const vec from_vec) : contents(from_vec), m(m), n(n){
        assert((int)from_vec.size() == m*n, "Input vector must fill give m*n matrix!");
    }

    Matrix(Vector from_vec, bool transposed = false) : 
        contents(from_vec), 
        m(!transposed ? from_vec.Size() : 1),
        n( transposed ? from_vec.Size() : 1)
    {}

    void DebugDisplay(const char* name) const{
        printf("Matrix %s:\n", name);
        for (int _m = 0; _m < m; _m++)
        {
            printf("\t");
            for (int _n = 0; _n < n; _n++)
            {
                printf("| %f ", Get(_m, _n));
            }
            printf("|\n");
        }
        printf("\\Matrix\n\n");
    }

    static const Matrix Identity(const int size){
        assert(size > 0, "Matrix size must be bigger than 0");
        vec result(size*size, 0);
        for (int i = 0; i < size; i++)
        {
            // nth row, nth column
            result[size * i + i] = 1;
        }
        return Matrix(size, size, result);
    }

    const Matrix Transpose() const{
        vec transposed_contents; transposed_contents.reserve(m*n);
        for (int _n = 0; _n < n; _n++)
        {
            for (int _m = 0; _m < m; _m++)
            {
                // target = _m*n + _n
                transposed_contents.push_back(contents[_m*n + _n]);
            }
        }
        return Matrix(n,m,transposed_contents);
    }

    static float GetIdx(int y, int x, int n) {
        assert(y >= 0, "Row must not be negative!");
        assert(x >= 0, "column must not be negative!");
        assert(n >  0, "Height must be bigger than 0!");
        return y * n + x;
    }

    const float Get(int y, int x) const{
        return contents[GetIdx(y, x, n)];
    }

    const Matrix operator*(const Matrix rhs) const{
        assert(n == rhs.m, "Invalid multiplication!");
        int N = n;
        int res_m = m, res_n = rhs.n;
        vec result; result.reserve(res_m*res_n);

        for (int _m = 0; _m < res_m; _m++)
        {
            for (int _n = 0; _n < res_n; _n++)
            {
                // Cell _m*res_n + _n
                int lhs_m = _m;
                int rhs_n = _n;

                float val = 0;
                for (int i = 0; i < N; i++)
                {
                    float v0 = Get(lhs_m, i);
                    float v1 = rhs.Get(i, rhs_n);
                    val += v0*v1;
                }
                result.push_back(val);
            }
        }
        return Matrix(res_m, res_n, result);
    }

    // Matrix * Vector(ColumnOriented) = Vector
    const Vector operator*(const vec rhs) const{
        const Matrix _rhs(rhs.size(), 1, rhs);
        return (*this * _rhs).ExtractColumn(0);
    }

    const Matrix operator+(const Matrix rhs) const{
        assert(m == rhs.m, "Matrix dimensions must equal!");
        assert(n == rhs.n, "Matrix dimensions must equal!");

        vec result; result.reserve(m*n);
        for (int _m = 0; _m < m; _m++)
        {
            for (int _n = 0; _n < n; _n++)
            {
                result.push_back(Get(_m, _n) + rhs.Get(_m, _n));
            }
        }
        return Matrix(m, n, result);
    }

    const Matrix operator-(const Matrix rhs) const{
        assert(m == rhs.m, "Matrix dimensions must equal!");
        assert(n == rhs.n, "Matrix dimensions must equal!");

        vec result; result.reserve(m*n);
        for (int _m = 0; _m < m; _m++)
        {
            for (int _n = 0; _n < n; _n++)
            {
                result.push_back(Get(_m, _n) - rhs.Get(_m, _n));
            }
        }
        return Matrix(m, n, result);
    }

    const Matrix operator*(const float rhs) const{
        vec result; result.reserve(m*n);
        for (int _m = 0; _m < m; _m++)
        {
            for (int _n = 0; _n < n; _n++)
            {
                result.push_back(Get(_m, _n) * rhs);
            }
        }
        return Matrix(m, n, result);
    }

    const Matrix operator/(const float rhs) const{
        float x = 1/rhs;
        return *this * x;
    }

    const vec ExtractColumn(const int column) const{
        assert(column >= 0, "Column must not be negative!");
        assert(column < n, "Column must be whitin range of matrix!");
        vec result; result.reserve(m);
        for (int _m = 0; _m < m; _m++)
        {
            result.push_back(Get(_m, column));
        }
        return result;
    }

    const vec ExtractRow(const int row) const{
        assert(row >= 0, "Row must not be negative!");
        assert(row < m,  "Row must be whitin range of matrix!");
        vec result; result.reserve(n);
        for (int _n = 0; _n < n; _n++)
        {
            result.push_back(Get(row, _n));
        }
        return result;
    }

    const Matrix Insert(const Matrix toInsert, int startCol, int startRow) const{
        if (startRow < 0) startRow = m - toInsert.m;
        if (startCol < 0) startCol = n - toInsert.n;

        assert(toInsert.m <= m, "Inserted matrix must be smaller than recipient.");
        assert(toInsert.n <= n, "Inserted matrix must be smaller than recipient.");

        assert(startRow + toInsert.m <= m, "Inserted matrix must be positioned inside recipient.");
        assert(startCol + toInsert.n <= n, "Inserted matrix must be positioned inside recipient.");

        // If we are gonna override the entire matrix. Dont.
        if (toInsert.m == m && toInsert.n == n) return toInsert;

        vec result(contents); // Clone recipient
        for (int _m = startRow; _m < startRow + toInsert.m; _m++)
        {
            for (int _n = startCol; _n < startCol + toInsert.n; _n++)
            {
                result[GetIdx(_m, _n, n)] = toInsert.Get(_m - startRow, _n - startCol);
            }
        }
        return Matrix(m, n, result);
    }

    const Matrix Subsection(const int startCol, int endCol, const int startRow, int endRow) const{
        if (endCol < 0) endCol = n-1;
        if (endRow < 0) endRow = m-1;

        assert(startCol >= 0, "Column cannot be negative!");
        assert(startRow >= 0, "Row cannot be negative!");

        assert(endCol < n, "Column must be whitin range!");
        assert(endRow < m, "Row must be whitin range!");

        assert(startCol <= endCol, "End cannot be before start!");
        assert(startRow <= endRow, "End cannot be before start!");

        const int M = (endRow - startRow + 1);
        const int N = (endCol - startCol + 1);

        vec result(M*N, 0);
        for (int _m = startRow; _m <= endRow; _m++)
        {
            const int _M = _m - startRow;
            for (int _n = startCol; _n <= endCol; _n++)
            {
                const int _N = _n - startCol;
                result[GetIdx(_M, _N, N)] = Get(_m, _n);
            }
        }
        return Matrix(M, N, result);
    }

    bool IsUpperTriangular() const{
        for (int _m = 0; _m < m; _m++)
        {
            for (int _n = 0; _n < n; _n++)
            {
                // Under the diagonal is not zero.
                if (_n < _m && !floatIsZero(Get(_m, _n))) return false;

                // Diagonal cannot be 0.
                if (_n == _m && floatIsZero(Get(_m, _n))) return false;
            }
            
        }
        return true;
    }
};

const Matrix operator*(float lhs, const Matrix rhs){
    return rhs * lhs;
}

//------------------------------------------------------------------------------------------------

const vector<Point> vector_diff_along_axis0(const vector<Point> points){
    assert(points.size() >= 2, "Not enough points to calculate diff!");

    vector<Point> result;
    result.reserve(points.size() - 1);
    for (size_t i = 0; i < points.size() -1; i++){
        result.push_back(points[i] - points[i+1]);
    }
    return result;
}

const vec FrobeniusNorm_for_points_axis1(const vector<Point> points){
    assert(points.size() >= 2, "Not enough points to calculate FrobeniusNorm!");

    vec result(points.size(), 0);
    for(size_t i = 0; i < points.size(); i++){
        result[i] = points[i].len();
    }
    return result;
}

const vec float_cumsum(const vec lenghts){
    assert(lenghts.size() >= 1, "Not enough values to calculate cumsum! At least 1 is required.");

    vec cumsum;
    cumsum.reserve(lenghts.size());
    cumsum.push_back(lenghts[0]); // First is unchanged

    // From the second to the last one.
    for (size_t i = 1; i < lenghts.size(); i++)
    {
        cumsum.push_back(cumsum[i-1] + lenghts[i]);
    }
    return cumsum;
}

const vec chord_lenght_parameterize(const vector<Point> points){
    assert(points.size() >= 2, "Not enough points to parameterize chord length!");

    const vec chord_lenghts = 
            FrobeniusNorm_for_points_axis1(
                vector_diff_along_axis0(points));
    
    vec result(float_cumsum(chord_lenghts));
    result.insert(result.begin(), 0);
    for (size_t i = 0; i < result.size(); i++)
    {
        result[i] = result[i] / result.back();
    }

    return result;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Matrix calculate_householder_matrix(const Vector a){
    vec _e(a.Size(), 0); 
    _e[0] = a.Magnitude() * sign(a.Get(0));
    Vector e(_e);
    Vector w = (a - e).Normalize();
    return Matrix::Identity(a.Size()) - 2 * Matrix(w) * Matrix(w, true); 
}

const pair<const Matrix, const Matrix> calculate_nextA(const Matrix A, const int iteration){
    /*
        x x x x x ...
        x o o o o ...
        x o o o o ...
        x o o o o ...
        ...
    */
    // -1 indicates subsection goes until the end of the row/column
    const Matrix SubA = A.Subsection(iteration, iteration, iteration, -1);
    const Vector a = SubA.ExtractColumn(0); // Only extract becouse subsection returns matrix

    const Matrix H = calculate_householder_matrix(a);
    const Matrix _H = Matrix::Identity(A.m).Insert(H, iteration, iteration);
    const Matrix _A = _H * A;
    return make_pair(_A, _H);
}

const pair<const Matrix, const Matrix> QR_decomposition(const Matrix A){
    vector<Matrix> Q_Storage; 
    Q_Storage.reserve(A.n); 
    Q_Storage.push_back(Matrix::Identity(A.m));

    vector<Matrix> A_Storage;
    A_Storage.reserve(A.n); 
    A_Storage.push_back(A); // Insert first A

    for (int i = 0; i < A.n; i++)
    {
        // Start the new iteration with the latest A`.
        auto AQ_Pair = calculate_nextA(A_Storage.back(), i);
        A_Storage.push_back(AQ_Pair.first);
        Q_Storage.push_back(Q_Storage.back() * AQ_Pair.second.Transpose());
    }
    return make_pair( Q_Storage.back(), A_Storage.back() );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Vector back_substitution(const Matrix R, const Vector b){
    assert(R.IsUpperTriangular(), "R must be upper triangular!");
    assert(R.m == (int)b.Size(), "R must be as tall as b");
    
    // rmn * xn = bm => xn = bm / rnm
    // 

    vec x; 
    x.reserve(R.n); // x growns backwards,
    for (int tier = 0; tier < R.n; tier++)
    {
        int idx = R.n-1-tier;
        int bIdx = idx;

        // Current Diagonal
        int Rm = idx;
        int Rn = idx;

        float _x = 0;
        for (int i = Rn; i < R.n ; i++)
        {
            // Diagonal, we know this
            if (i == Rm) {
                _x += b.Get(bIdx) / R.Get(Rm, Rn);
                continue;
            }
            
            int curX = idx - i-Rn; 
            // Previous Xs
            _x += x[curX];
        }
        x.push_back(_x);
    }
    return Vector(x).Reverse();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

const Bezier FitCubicBezier(const vector<Point> points){
    assert(points.size() >= 2, "Not enough points to fit cubic bezier!");

    if (points.size() == 2)
        return Bezier(points[0],points[0],points[1],points[1]);
    
    const vec t(chord_lenght_parameterize(points));
    const Point P0 = points.front();
    const Point P3 = points.back();
    
    vec _A;    _A.reserve(points.size());
    vec bx;   bx.reserve(points.size());
    vec by;   by.reserve(points.size());
    
    for (size_t i = 0; i < points.size(); i++)
    {
        float ti = t[i];
        float a = 3 * powf(1 - ti, 2) * ti;
        float b = 3 * (1 - ti) * ti*ti;
        const Point c = P0 * powf(1 - ti, 3) + P3 * ti*ti*ti;
        
        _A.push_back(a);
        _A.push_back(b);
        bx.push_back(points[i].x - c.x);
        by.push_back(points[i].y - c.y);
    }
    
    const Matrix A(points.size(), 2, _A);

    auto QR_Pair = QR_decomposition(A);
    const Matrix Q = QR_Pair.first;
    const Matrix R = QR_Pair.second.Subsection(0, 1, 0, 1); // Get the 2x2 upper triangular matrix
    
    // R * x = Q^T * bx
    // R * y = Q^T * by
    const Matrix QT = Q.Transpose();
    const Vector QBx = (QT * bx).Subsection(0, 2); // Discard unnesesery part.
    const Vector QBy = (QT * by).Subsection(0, 2);
    
    // R * x = QBx => BackSubstitution
    // R * y = QBy => BackSubstitution
    const Vector X = back_substitution(R, QBx);
    const Vector Y = back_substitution(R, QBy);


    return Bezier(P0, Point(X.Get(0), Y.Get(0)), Point(X.Get(1), Y.Get(1)), P3);
}

//------------------------------------------------------------------------------------------------

const Point BezierCubic(float t, const Bezier bezier){
    assert(t >= 0, "T must be in range of [0, 1]");
    assert(t <= 1, "T must be in range of [0, 1]");
    
    const Point P0 = bezier.P0;
    const Point P1 = bezier.P1;
    const Point P2 = bezier.P2;
    const Point P3 = bezier.P3;

    return  P0 * powf(1-t, 3) +
    P1 * 3 * powf(1-t, 2) * t +
    P2 * 3 * (1-t) * t*t +
    P3 * t*t*t;
}

double EvaluateBezier(const Bezier bezier, const vector<Point> points){
    if (points.size() <= 2) return 0;
    const vec t(chord_lenght_parameterize(points));

    assert(t.size() == points.size(), "The number of Ts and points do not match.");

    double accumulated_error = 0;
    for (size_t i = 0; i < points.size(); i++)
    {
        Point evaluated = BezierCubic(t[i], bezier);
        accumulated_error += (double)(evaluated - points[i]).len();
    }   
    return accumulated_error;
}

//------------------------------------------------------------------------------------------------



#undef vec
#ifdef DEBUG_CF

int main(){
    vector<Point> points = {Point(-4.01,-1.7),Point(-3.64,-0.7),Point(-2.8,0.25),Point(-1.36,0.97),Point(0.05,1.52),Point(2.07,1.94),Point(2.89,2.11)};
    //Matrix m(7, 2, v);
    Bezier b = FitCubicBezier(points);
    b.P0.DebugDisplay("P0");
    b.P1.DebugDisplay("P1");
    b.P2.DebugDisplay("P2");
    b.P3.DebugDisplay("P3");

    double error = EvaluateBezier(b, points);
    printf("Error: %lf", error);
}

#endif