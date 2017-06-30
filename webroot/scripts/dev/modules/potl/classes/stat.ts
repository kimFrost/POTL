namespace POTLModule {
    export class Stat {
        public Id: string = '';
        public Title: string = '';
        public Percentage: number = 100;
        public MaxValue: number = 50000;
        public ConsumeMultiplier: number = 1;
        public Value: number = 50000;
        public Wants: Array<any> = [];
        public Importance: number = 1;
        constructor(
            Id: string,
            Title?: string,
        ) {
            this.Id = Id;
            this.Title = Title || Id;
        }
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

