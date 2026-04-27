<div align="center">

  <h1>📦 Mecalux Challenge | Hack UPC 2026</h1>
  
  <p>
    <strong>Solution developed for the Mecalux challenge during Hack UPC 2026</strong>
  </p>
  
  <p>
    <a href="https://github.com/tu-usuario/tu-repo/graphs/contributors">
      <img src="https://img.shields.io/badge/Contributors-4-blue?style=for-the-badge" alt="Contributors" />
    </a>
    <a href="https://img.shields.io/badge/Language-C++-blue?style=for-the-badge&logo=c%2B%2B">
      <img src="https://img.shields.io/badge/Language-C++-blue?style=for-the-badge&logo=c%2B%2B" alt="C++" />
    </a>
    <a href="https://img.shields.io/badge/Language-HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white">
      <img src="https://img.shields.io/badge/Language-HTML5-E34F26?style=for-the-badge&logo=html5&logoColor=white" alt="HTML5" />
    </a>
    <a href="https://hackupc.com/">
      <img src="https://img.shields.io/badge/Event-Hack_UPC_2026-black?style=for-the-badge" alt="Hack UPC 2026" />
    </a>
  </p>
</div>

---

# 📦 Mecalux Warehouse Optimizer

**Mecalux Warehouse Optimizer** is a calculation and simulation engine written in C++ for the optimal spatial distribution of storage bays within a warehouse.  
Instead of relying on manual layouts prone to spatial errors, our algorithm dynamically processes building dimensions, ceiling heights, and local obstacles to calculate the most efficient and cost-effective layout possible.

---

## Inspiration

Industrial warehouse design is often limited by complex architectures, pillars, and ceiling variations. We wanted to build a fast algorithmic model: what if a program could fit bays like a large-scale "Tetris", calculating collisions in real-time and optimizing load costs?

This project is inspired by the Mecalux challenge for Hack UPC 2026, focusing on 2D Bin Packing problems with 3D constraints (ceiling heights) and the need to prioritize the most cost-efficient storage modules.

---

## What it does

- Reads the warehouse topology through 4 input files: `warehouse.csv`, `obstacles.csv`, `ceiling.csv`, and `types_of_bays.csv`.
- Processes physical constraints: ensures bays do not collide with obstacles and respect the maximum allowed height at each coordinate (Ceiling Height).
- Calculates positions, allowing bays to share boundaries with each other and with the warehouse walls.
- Generates a solution aimed at placing the highest number of bays in the cheapest way using the largest amount of area possible.
- Exports a list of coordinates and rotations (Id, X, Y, Rotation) through standard output.
- Allows graphical visualization of the solution via a web interface.

## Setup and run

### Prepare the data
Modify the `.csv` files in the root folder with the values and coordinates you want to evaluate (input). Warehouse walls must always be axis-aligned, and obstacles will always be boxes.
- `warehouse.csv`: Contains the coordinates of the warehouse walls.
- `obstacles.csv`: Contains the coordinates and dimensions of the obstacles.
- `ceiling.csv`: Contains the coordinates and height of the ceiling.
- `types_of_bays.csv`: Contains the types of bays available to place.

---

### Compile and run the algorithm
Run the C++ compiler to generate the executable and run it:

```bash
g++ main.cc -o solver && ./solver > output.txt
``` 
---

### Visualize the solution

Since the visualizer is a static HTML page, it does not require a web server or compilation step.
1. Simply double-click the `visualizador.html` file in your file explorer, or open it directly in your web browser.
2. Inside the viewer, paste the data from your `.csv` files into their respective sections: `warehouse`, `ceiling`, `obstacles`, and `baytypes`.
3. In the section labeled `output`, paste the text you got from the terminal in step 2.
4. The viewer will proceed to draw an interactive 3D floor plan of the warehouse with the provided information.

---

### Map line meaning

The graphical simulation uses shapes and colors to indicate the state of the warehouse:

* **Solid black lines/polygons:** Represent the limits (walls) from `warehouse.csv`.
* **Gray/striped boxes:** Represent impassable obstacles from `obstacles.csv`.
* **Orange/blue rectangles:** Represent the bays placed in their final position, reflecting their width and depth.
* **Colored background areas:** Indicate the different ceiling height regions from `ceiling.csv`.

---

### How we built it

* **Algorithmic Core:** Standard C++ to ensure maximum performance when iterating through possible spatial combinations.
* **Data Model:** Flat structures (structs) to represent vertices (X, Y), bay characteristics (Id, Width, Depth, Height, nLoads, Price), and geometric interactions.
* **Optimization:** Implementation of heuristics to maximize the quality metric (Q) by evaluating the relationship between covered area and price per load: $Q = \left( \sum \frac{price}{loads} \right)^{2 - \frac{\sum area}{warehouse\_area}}$.
* **Frontend Bridge:** A built-in Python server and Vanilla JS / HTML Canvas to render plain text output into understandable graphics.

---

### Challenges we ran into

* Implementing an algorithm fast enough to ensure evaluation test runs do not exceed the 30-second limit.
* Logically modeling ceiling areas to discard bays whose height exceeded the limit on specific X-axis regions.
* Translating pure mathematical results from the terminal to a user-friendly web viewer to fulfill the "Presentation" judging criteria.

---

### Accomplishments that we're proud of

* Built a 2D packing engine with 3-dimensional dependencies from scratch in C++ over a single weekend.
* Successfully separated heavy calculation logic (C++ backend) from presentation logic (web frontend).
* Created a deterministic solution that takes advantage of the fact that bays can share adjacent coordinates without triggering collision.

---

### What we learned

* Dynamic memory management and loop optimization in C++ are crucial when evaluating thousands of possible placement coordinates.
* Having visual feedback (the web viewer) dramatically speeds up the debugging process for geometric coordinates and overlaps.
* A modular architecture (separating input CSVs, console-based calculation engine, and web UI) prevents blocking during team development.

---

### What's next

* Integrate Genetic Algorithms or Simulated Annealing to improve upon initial "Greedy" solutions and find even more optimal layouts.
* Automate the workflow by directly connecting the C++ output to the viewer via WebSockets, eliminating the "copy and paste" step.
* Add pre-calculation CSV format validators to prevent application crashes.
* Expand stress testing with massive datasets and much more complex warehouse polygons.