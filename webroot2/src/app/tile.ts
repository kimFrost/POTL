export class Tile {
    public x: number;
    public y: number;
    public neighbours: Tile[];
    public structure: any;
    constructor(
        x: number,
        y: number
    ) {
        this.x = x;
        this.y = y;
    }
}
