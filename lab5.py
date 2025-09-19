import math

def point_on_line(p, line_start, line_end):
    # Проверяет, лежит ли точка p на прямой, проходящей через line_start и line_end
    x, y = p
    x1, y1 = line_start
    x2, y2 = line_end

    v1 = (x - x1, y - y1)
    v2 = (x2 - x1, y2 - y1)

    cross = abs(v1[0] * v2[1] - v1[1] * v2[0])
    return cross < 1e-9


def lines_intersect(line1_start, line1_end, line2_start, line2_end):
    # Проверяет, пересекаются ли два отрезка
    def ccw(A, B, C):
        return (C[1] - A[1]) * (B[0] - A[0]) > (B[1] - A[1]) * (C[0] - A[0])

    A, B = line1_start, line1_end
    C, D = line2_start, line2_end

    return ccw(A, C, D) != ccw(B, C, D) and ccw(A, B, C) != ccw(A, B, D)


def is_convex_polygon(points):
    # Проверяет, является ли многоугольник выпуклым.
    if len(points) < 3:
        return False

    def cross_product(o, a, b):
        return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])

    n = len(points)
    signs = []

    for i in range(n):
        o = points[i]
        a = points[(i + 1) % n]
        b = points[(i + 2) % n]
        cp = cross_product(o, a, b)
        if abs(cp) < 1e-9:
            continue  # Коллинеарные точки — не считаем
        signs.append(cp > 0)

    # Все знаки должны быть одинаковыми (все повороты одинакового направления)
    return all(s == signs[0] for s in signs)


def circles_intersection(c1_center, c1_radius, c2_center, c2_radius):
    #Возвращает количество точек пересечения двух окружностей. (0,1,2)
    x1, y1 = c1_center
    x2, y2 = c2_center
    r1, r2 = c1_radius, c2_radius

    d = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)

    
    if d > r1 + r2:
        return 0
    if d < abs(r1 - r2):
        return 0

    if d == 0 and r1 == r2:
        return 0  # Совпадают — считаем как 0 пересечений
    if d == r1 + r2 or d == abs(r1 - r2):
        return 1  # Касаются
    if d < r1 + r2 and d > abs(r1 - r2):
        return 2  # Пересекаются в двух точках

    return 0


def main():
    # Примеры использования
    print("=== Тесты ===")

    # 1. Точка на прямой?
    p = (1, 1)
    l1 = (0, 0)
    l2 = (2, 2)
    print(f"Точка {p} на прямой через {l1} и {l2}: {point_on_line(p, l1, l2)}")

    # 2. Пересекаются ли две прямые?
    l1_start = (0, 0)
    l1_end = (2, 2)
    l2_start = (0, 2)
    l2_end = (2, 0)
    print(f"Прямые пересекаются: {lines_intersect(l1_start, l1_end, l2_start, l2_end)}")

    # 3. Выпуклый многоугольник?
    poly1 = [(0, 0), (1, 0), (1, 1), (0, 1)]  # квадрат — выпуклый
    poly2 = [(0, 0), (2, 0), (1, 1), (1, 2)]  # зигзаг — вогнутый
    print(f"Многоугольник {poly1} выпуклый: {is_convex_polygon(poly1)}")
    print(f"Многоугольник {poly2} выпуклый: {is_convex_polygon(poly2)}")

    # 4. Пересечение окружностей
    c1 = (0, 0)
    r1 = 2
    c2 = (3, 0)
    r2 = 2
    print(f"Окружности пересекаются {circles_intersection(c1, r1, c2, r2)} раз(а)")


if __name__ == "__main__":
    main()