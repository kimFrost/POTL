
import { Provider } from './provider';

export class Tile {
    public x: number;
    public y: number;
    public neighbours: Tile[];
    public providers: Provider[];
    constructor(
        x: number,
        y: number
    ) {
        this.x = x;
        this.y = y;
        this.neighbours = [];
        this.providers = [];
    }
}
