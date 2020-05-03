module grid() {
    linear_extrude(height = 12, center = true, convexity = 10)
        import(file = "Grid.svg", layer = "plate", dpi = 72);
}

grid();
