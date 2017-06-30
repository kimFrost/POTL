namespace POTLModule {
    export interface INeed {
        Id: string;
        Title: string;
        Importance?: number;
        Wants?: Array<any>;
        Value?: number;
    }
    export class Need implements INeed {
        public Percentage: number = 100;
        public MaxValue: number = 50000;
        public ConsumeMultiplier: number = 1;
        public Value: number = 50000;
        public Wants: Array<any> = [];
        constructor(
            public Id: string = '',
            public Title: string = '',
            public Importance: number = 0
        ) { }
        public add(amount: number): void {
            this.Value += amount;
            this.clampValue();
            this.updatePercentage();
        }
        public subtract(amount: number): void {
            this.Value -= amount;
            this.clampValue();
            this.updatePercentage();
        }
        public clampValue(): void {
            if (this.Value < 0) {
                this.Value = 0;
            }
            else if (this.Value > this.MaxValue) {
                this.Value = this.MaxValue;
            }
            this.Value = Math.ceil(this.Value);
        }
        public updatePercentage(): number {
            this.Percentage = Math.ceil(this.Value / this.MaxValue * 100);
            return this.Percentage;
        }
    }
}

