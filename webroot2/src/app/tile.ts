
import { Provider } from './provider';
import { Structure } from './structure';

export class Tile {
    public x: number;
    public y: number;
    public neighbours: Tile[];
    public providers: Provider[];
    public structure: Structure;
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
