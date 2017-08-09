import { TestBed, inject } from '@angular/core/testing';

import { TileManagerService } from './tile-manager.service';

describe('TileManagerService', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [TileManagerService]
    });
  });

  it('should be created', inject([TileManagerService], (service: TileManagerService) => {
    expect(service).toBeTruthy();
  }));
});
