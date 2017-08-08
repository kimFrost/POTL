import { async, ComponentFixture, TestBed } from '@angular/core/testing';

import { OverlaySwitcherComponent } from './overlay-switcher.component';

describe('OverlaySwitcherComponent', () => {
  let component: OverlaySwitcherComponent;
  let fixture: ComponentFixture<OverlaySwitcherComponent>;

  beforeEach(async(() => {
    TestBed.configureTestingModule({
      declarations: [ OverlaySwitcherComponent ]
    })
    .compileComponents();
  }));

  beforeEach(() => {
    fixture = TestBed.createComponent(OverlaySwitcherComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should be created', () => {
    expect(component).toBeTruthy();
  });
});
