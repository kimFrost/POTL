
import { Provider } from './provider';

export class Structure {
    public providers: Provider[];
    public structure: any;
    constructor(
        providers?: Provider[]
    ) {
        this.providers = providers || [];
    }
}
